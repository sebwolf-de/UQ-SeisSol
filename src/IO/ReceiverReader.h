#pragma once

#include "SeisSol/Receiver.h"
#include <string>
#include <map>

namespace IO {

class ReceiverReader {
  public:
  ReceiverReader(std::string dir, std::string prefix = "receiver");
  void parseReceiver(size_t number, SeisSol::Receiver& receiver) const;
  void parseReceiver(std::string fileName, SeisSol::Receiver& receiver) const;

  private:
  const std::string observationsDirectory;
  const std::string receiverPrefix;
};

std::map<size_t, std::string> getReceiversInDirectory(std::string directory, std::string prefix);

} // namespace IO
