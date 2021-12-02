#include "ReceiverReader.h"
#include <boost/filesystem.hpp>
#include <cassert>
#include <cmath>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <map>

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

void IO::ReceiverReader::parseReceiver(size_t number, SeisSol::Receiver& receiver, size_t fsn) const {
  const auto receiverList = getReceiversInDirectory(observationsDirectory, receiverPrefix);
  const auto fileName = receiverList.at(number);
  parseReceiver(fileName, receiver, fsn);
}

void IO::ReceiverReader::parseReceiver(std::string fileName, SeisSol::Receiver& receiver, size_t fsn) const {
  std::ifstream in(fileName);
  std::string line;

  // read first five lines
  // TODO read first line to check whether the receiver number of the file and the requested number
  // coincide
  std::getline(in, line);
  std::getline(in, line);
  std::string headline = line;
  // assert("VARIABLES = \"Time\",\"xx\",\"yy\",\"zz\",\"xy\",\"yz\",\"xz\",\"u\",\"v\",\"w\"" ==
  //        line);
  // VARIABLES = "Time","xx0","yy0","zz0","xy0","yz0","xz0","u0","v0","w0","xx1","yy1","zz1","xy1","yz1","xz1","u1","v1","w1","xx2","yy2","zz2","xy2","yz2","xz2","u2","v2","w2","xx3","yy3","zz3","xy3","yz3","xz3","u3","v3","w3"

  // TODO extract receiver location
  for (size_t i = 0; i < 3; i++) {
    std::getline(in, line);
  }

  auto parseLineFused = [](std::string line, size_t fsn) {
    std::array<double, 10> elems;
    std::stringstream ss(line);
    std::string item;
    size_t i = 0;

    if (fsn > 1) {
      // extract time information
      ss >> item;
      double candidate = std::stod(item);
      if (!std::isfinite(candidate)) {
        throw std::invalid_argument("Found non-finite value in receiver input.");
      }
      elems[i] = candidate;
      i++;

      // get to the part of interest
      for (size_t j = 0; j < (fsn-1)*9; j++)
      {
        ss >> item;
      }
    }
  
    while (ss >> item) {
      if (i > 9) {
          break;
        }
      double candidate = std::stod(item);
      if (!std::isfinite(candidate)) {
        throw std::invalid_argument("Found non-finite value in receiver input.");
      }
      elems[i] = candidate;
      i++;
    }

    return elems;
  };

  receiver.clear();

  while (std::getline(in, line)) {
    const auto parsedLine = parseLineFused(line, fsn);
    receiver.appendData(parsedLine);
  }
}
