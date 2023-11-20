#include "ReceiverReader.h"
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <string>

#include "spdlog/spdlog.h"
#include <iostream>

IO::ReceiverReader::ReceiverReader(std::string dir, std::string prefix)
    : observationsDirectory(std::move(dir)), receiverPrefix(std::move(prefix)) {}

std::map<size_t, std::string> IO::getReceiversInDirectory(std::string directory,
                                                          std::string prefix) {
  std::map<size_t, std::string> fileList;
  if (!std::filesystem::exists(directory)) {
    return fileList;
  }

  const std::regex matcher(prefix + "-([0-9]{5})-.{5}\\.dat");

  for (const std::filesystem::directory_entry& e : std::filesystem::directory_iterator(directory)) {
    if (!std::filesystem::is_directory(e.path())) {
      const auto fileName = e.path().filename().string();
      std::smatch m;
      if (std::regex_match(fileName, m, matcher)) {
        const auto filePath = e.path().string();
        try {
          fileList.insert({std::stoi(m[1]), filePath});
        } catch (const std::exception& e) {
          // m[1] und fileName loggen
          spdlog::debug("ReceiverReader - m[1]: {} with fileName: {}", m.str(1), fileName);
          std::cerr << e.what() << '\n';
        }
      }
    }
  }
  return fileList;
}

void IO::ReceiverReader::parseReceiver(size_t number, SeisSol::Receiver& receiver,
                                       size_t fsn) const {
  const auto receiverList = getReceiversInDirectory(observationsDirectory, receiverPrefix);
  const auto fileName = receiverList.at(number);
  parseReceiver(fileName, receiver, fsn);
}

void IO::ReceiverReader::parseReceiver(std::string fileName, SeisSol::Receiver& receiver,
                                       size_t fsn) {
  std::ifstream in(fileName);
  std::string line;

  // read first five lines
  // TODO read first line to check whether the receiver number of the file and the requested number
  // coincide
  std::getline(in, line);
  std::getline(in, line);
  const std::string headline = line;
  // assert("VARIABLES = \"Time\",\"xx\",\"yy\",\"zz\",\"xy\",\"yz\",\"xz\",\"u\",\"v\",\"w\"" ==
  //        line);
  // VARIABLES =
  // "Time","xx0","yy0","zz0","xy0","yz0","xz0","u0","v0","w0","xx1","yy1","zz1","xy1","yz1","xz1","u1","v1","w1","xx2","yy2","zz2","xy2","yz2","xz2","u2","v2","w2","xx3","yy3","zz3","xy3","yz3","xz3","u3","v3","w3"

  // TODO extract receiver location
  for (size_t i = 0; i < 3; i++) {
    std::getline(in, line);
  }

  auto parseLineFused = [](std::string line, size_t fsn) {
    std::array<double, SeisSol::Receiver::lineWidth> elems{};
    std::stringstream ss(line);
    std::string item;
    size_t i = 0;

    if (fsn > 1) {
      // extract time information
      ss >> item;
      const double candidate = std::stod(item);
      if (!std::isfinite(candidate)) {
        throw std::invalid_argument("Found non-finite value in receiver input.");
      }
      elems.at(i) = candidate;
      i++;

      // get to the part of interest
      for (size_t j = 0; j < (fsn - 1) * SeisSol::Receiver::numberOfQuantities; j++) {
        ss >> item;
      }
    }

    while (ss >> item) {
      if (i > SeisSol::Receiver::numberOfQuantities) {
        break;
      }
      const double candidate = std::stod(item);
      if (!std::isfinite(candidate)) {
        throw std::invalid_argument("Found non-finite value in receiver input.");
      }
      elems.at(i) = candidate;
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
