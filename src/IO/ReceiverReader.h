#pragma once

#include "SeisSol/Receiver.h"
#include <string>

namespace IO {

class ReceiverReader {
  public:
  ReceiverReader(std::string dir, std::string prefix = "receiver");
  void parseReceiver(size_t number, SeisSol::Receiver& receiver) const;

  private:
  const std::string observationsDirectory;
  const std::string receiverPrefix;
};

} // namespace IO
