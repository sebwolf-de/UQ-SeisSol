#pragma once

#include <map>

#include "IO/ReceiverReader.h"
#include "Receiver.h"
#include <vector>

namespace SeisSol {

class ReceiverDB {
  public:
  ReceiverDB(std::string directory, std::string prefix);
  void addReceiver(size_t i);
  const SeisSol::Receiver& getReceiver(size_t) const;
  std::vector<double> l1Difference(size_t i, const SeisSol::Receiver& otherReceiver, size_t numberOfSubintervals) const;
  size_t numberOfReceivers() const;

  private:
  std::map<int, SeisSol::Receiver> receivers;
  const IO::ReceiverReader receiverReader;
};

} // namespace SeisSol
