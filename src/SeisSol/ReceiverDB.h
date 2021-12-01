#pragma once

#include <map>

#include "IO/ReceiverReader.h"
#include "Receiver.h"
#include <vector>

namespace SeisSol {

class ReceiverDB {
  public:
  ReceiverDB(std::string directory, std::string prefix, size_t fsn);
  void addReceiver(size_t i, size_t fsn);
  void addAllReceivers(size_t fsn);
  const SeisSol::Receiver& getReceiver(size_t i, size_t fsn) const;
  std::vector<double> l1Difference(size_t i, const SeisSol::Receiver& otherReceiver, size_t numberOfSubintervals, size_t fsn) const;
  size_t numberOfReceivers(size_t fsn) const;

  private:
  auto receivers; // std::vector<std::map<int, SeisSol::Receiver>>
  const IO::ReceiverReader receiverReader;
  std::string directory;
  std::string prefix;
};

} // namespace SeisSol
