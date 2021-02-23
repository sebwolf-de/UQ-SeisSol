#pragma once

#include <map>

#include "IO/ReceiverReader.h"
#include "Receiver.h"

namespace SeisSol {

class ReceiverDB {
  public:
  ReceiverDB(std::string directory, std::string prefix);
  void addReceiver(size_t i);
  const SeisSol::Receiver& getReceiver(size_t) const;
  double l1Difference(size_t i, const SeisSol::Receiver& otherReceiver) const;
  const size_t numberOfReceivers() const;

  private:
  std::map<int, SeisSol::Receiver> receivers;
  const IO::ReceiverReader receiverReader;
};

} // namespace SeisSol
