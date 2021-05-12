#include "ReceiverDB.h"
#include "SeisSol/Receiver.h"
#include "IO/ReceiverReader.h"
#include "SeisSol/Runner.h"
#include <vector>


SeisSol::ReceiverDB::ReceiverDB(std::string directory, std::string prefix)
    : directory(directory), prefix(prefix), receiverReader(IO::ReceiverReader(directory, prefix)) {}

void SeisSol::ReceiverDB::addReceiver(size_t i) {
  SeisSol::Receiver receiver;
  receiverReader.parseReceiver(i, receiver);
  receivers[i] = receiver;
}

void SeisSol::ReceiverDB::addAllReceivers() {
  const auto fileNames = IO::getReceiversInDirectory(directory, prefix);
  for (const auto& entry: fileNames) {
    SeisSol::Receiver receiver;
    receiverReader.parseReceiver(entry.second, receiver);
    receivers[entry.first] = receiver;
  }
}

std::vector<double> SeisSol::ReceiverDB::l1Difference(size_t i, const SeisSol::Receiver& otherReceiver, size_t numberOfSubintervals) const {
  return receivers.at(i).l1Difference(otherReceiver, numberOfSubintervals);
}

const SeisSol::Receiver& SeisSol::ReceiverDB::getReceiver(size_t i) const {
  return receivers.at(i);
}

size_t SeisSol::ReceiverDB::numberOfReceivers() const { return receivers.size(); }
