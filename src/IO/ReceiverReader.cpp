#include "ReceiverReader.h"
#include <boost/filesystem.hpp>
#include <cassert>
#include <cmath>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <map>
#include <iostream>

IO::ReceiverReader::ReceiverReader(std::string dir, std::string prefix)
    : observationsDirectory(dir), receiverPrefix(prefix) {}

std::map<size_t, std::string> IO::getReceiversInDirectory(std::string directory, std::string prefix) {
  std::map<size_t, std::string> fileList;
  if (!boost::filesystem::exists(directory)) {
    return fileList;
  }

  std::string fileName;
  std::regex matcher(prefix + "-([0-9])*-.*\\.dat");

  for (boost::filesystem::directory_entry& e :
       boost::filesystem::directory_iterator(directory)) {
    if (!boost::filesystem::is_directory(e.path())) {
      std::smatch m;
      if (std::regex_match(e.path().filename().string(), m,  matcher)) {
        fileName = e.path().string();
        fileList.insert({std::stoi(m[1]), fileName});
      }
    }
  }
  return fileList;
}

void IO::ReceiverReader::parseReceiver(size_t number, SeisSol::Receiver& receiver) const {
  const auto receiverList = getReceiversInDirectory(observationsDirectory, receiverPrefix);
  const auto fileName = receiverList.at(number);

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
    std::array<double, 10> elems;
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
        elems[i] = candidate;
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
