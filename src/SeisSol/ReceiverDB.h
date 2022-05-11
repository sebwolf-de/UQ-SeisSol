#pragma once

#include <map>

#include "IO/ReceiverReader.h"
#include "Receiver.h"
#include <vector>

namespace SeisSol {

class ReceiverDB {
  public:
  ReceiverDB(std::string directory, std::string prefix, size_t fsn = 1);
  void addReceiver(size_t i, size_t fsn = 1);
  void addAllReceivers(size_t fsn = 1);
  const SeisSol::Receiver& getReceiver(size_t i, size_t fsn = 1) const;
  std::vector<double> l1Difference(size_t i, const SeisSol::Receiver& otherReceiver,
                                   size_t numberOfSubintervals, size_t fsn = 1) const;
  size_t numberOfReceivers(size_t fsn = 1) const;

  private:
  std::map<int, std::map<int, SeisSol::Receiver>> receivers;
  const IO::ReceiverReader receiverReader;
  std::string directory;
  std::string prefix;
};

} // namespace SeisSol
