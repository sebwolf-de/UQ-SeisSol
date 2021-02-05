#include "ReceiverReader.h"
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

Reader::ReceiverReader::ReceiverReader(std::string dir, std::string prefix) : 
  observationsDirectory(dir), receiverPrefix(prefix) {};


void Reader::ReceiverReader::getReceiver(size_t number, SeisSol::Receiver& receiver) const {
  //TODO don't make this hardcoded
  const std::string fileName = observationsDirectory + "/" + receiverPrefix + "-0000" + std::to_string(number) + "-00000.dat";
  std::ifstream in(fileName);
  std::string line;

  // read first five lines
  // TODO read first line to check whether the receiver number of the file and the requested number coincide
  std::getline(in, line);
  std::getline(in, line);
  assert("VARIABLES = \"Time\",\"xx\",\"yy\",\"zz\",\"xy\",\"yz\",\"xz\",\"u\",\"v\",\"w\"" == line);

  //TODO extract receiver location
  for (size_t i = 0; i < 3; i++) {
    std::getline(in, line);
  }

  auto parseLine = [](std::string line) {
    std::array<double, 10> elems;
    std::stringstream ss(line);
    std::string item;
    size_t i = 0;
    while (std::getline(ss, item, ' ')) {
      try {
      elems[i] = std::stod(item);
      i++;
      } catch (std::exception e) {}
    }
    return elems;
  };

  receiver.clear(); 

  while (std::getline(in, line)) {
    const auto parsedLine = parseLine(line);
    receiver.appendData(parsedLine);
  }
}






