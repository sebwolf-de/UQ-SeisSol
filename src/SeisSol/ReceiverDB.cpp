#include "ReceiverDB.h"
#include "SeisSol/Receiver.h"
#include "IO/ReceiverReader.h"
#include "SeisSol/Runner.h"
#include <vector>


SeisSol::ReceiverDB::ReceiverDB(std::string directory, std::string prefix, size_t fsn = 1)
    : receiverReader(IO::ReceiverReader(directory, prefix)), directory(directory), prefix(prefix) {
      receivers = std::array<std::map<int, SeisSol::Receiver>, fsn>();
    }

void SeisSol::ReceiverDB::addReceiver(size_t i, size_t fsn = 1) {
  SeisSol::Receiver receiver;
  receiverReader.parseReceiver(i, receiver);
  receivers[fsn][i] = receiver;
}

void SeisSol::ReceiverDB::addAllReceivers(size_t fsn = 1) {
  const auto fileNames = IO::getReceiversInDirectory(directory, prefix);
  for (const auto& entry: fileNames) {
    SeisSol::Receiver receiver;
    receiverReader.parseReceiver(entry.second, receiver);
    receivers[fsn][entry.first] = receiver;
  }
}

std::vector<double> SeisSol::ReceiverDB::l1Difference(size_t i, const SeisSol::Receiver& otherReceiver, size_t numberOfSubintervals, size_t fsn = 1) const {
  return receivers[fsn].at(i).l1Difference(otherReceiver, numberOfSubintervals);
}

const SeisSol::Receiver& SeisSol::ReceiverDB::getReceiver(size_t i, size_t fsn = 1) const {
  return receivers[fsn].at(i);
}

size_t SeisSol::ReceiverDB::numberOfReceivers(size_t fsn = 1) const { return receivers[fsn].size(); }
