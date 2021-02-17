#include "ReceiverReader.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

IO::ReceiverReader::ReceiverReader(std::string dir, std::string prefix)
    : observationsDirectory(dir), receiverPrefix(prefix){};

void IO::ReceiverReader::parseReceiver(size_t number, SeisSol::Receiver& receiver) const {
  // TODO don't make this hardcoded
  const std::string fileName = observationsDirectory + "/" + receiverPrefix + "-0000" +
                               std::to_string(number) + "-00000.dat";
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
