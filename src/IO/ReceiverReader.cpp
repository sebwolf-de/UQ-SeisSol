#include "ReceiverReader.h"
#include <boost/filesystem.hpp>
#include <cassert>
#include <cmath>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

IO::ReceiverReader::ReceiverReader(std::string dir, std::string prefix)
    : observationsDirectory(std::move(dir)), receiverPrefix(std::move(prefix)) {}

void IO::ReceiverReader::parseReceiver(size_t number, SeisSol::Receiver& receiver) const {
  assert(boost::filesystem::exists(observationsDirectory));

  std::string fileName;
  std::regex matcher(receiverPrefix + "-0*" + std::to_string(number) + "-.*\\.dat");

  for (boost::filesystem::directory_entry& e :
       boost::filesystem::directory_iterator(observationsDirectory)) {
    if (!boost::filesystem::is_directory(e.path())) {
      if (std::regex_match(e.path().filename().string(), matcher)) {
        fileName = e.path().string();
        break;
      }
    }
  }

  std::ifstream in(fileName);
  std::string line;

  // read first five lines
  // TODO read first line to check whether the receiver number of the file and the requested number
  // coincide
  std::getline(in, line);
  std::getline(in, line);
  assert("VARIABLES = \"Time\",\"xx\",\"yy\",\"zz\",\"xy\",\"yz\",\"xz\",\"u\",\"v\",\"w\"" ==
         line);

  // TODO extract receiver location
  for (size_t i = 0; i < 3; i++) {
    std::getline(in, line);
  }

  auto parseLine = [](std::string line) {
    std::array<double, 10> elems {};
    std::stringstream ss(line);
    std::string item;
    size_t i = 0;

    while (std::getline(ss, item, ' ')) {
      // if there are several whitespaces after one another as a delimiter, ignore these.
      if (item == std::string("")) {
        continue;
      } else {
        double candidate = std::stod(item);
        if (!std::isfinite(candidate)) {
          throw std::invalid_argument("Found non-finite value in receiver input.");
        }
        elems.at(i) = candidate;
        i++;
      }
    }

    return elems;
  };

  receiver.clear();

  while (std::getline(in, line)) {
    const auto parsedLine = parseLine(line);
    receiver.appendData(parsedLine);
  }
}
