#include "ReceiverDB.h"
#include "SeisSol/Receiver.h"

SeisSol::ReceiverDB::ReceiverDB(std::string directory, std::string prefix)
    : receiverReader(IO::ReceiverReader(std::move(directory), std::move(prefix))) {}

void SeisSol::ReceiverDB::addReceiver(size_t i) {
  SeisSol::Receiver receiver;
  receiverReader.parseReceiver(i, receiver);
  receivers.insert({i, receiver});
}

double SeisSol::ReceiverDB::l1Difference(size_t i, const SeisSol::Receiver& otherReceiver) const {
  return receivers.at(i).l1Difference(otherReceiver);
}

const SeisSol::Receiver& SeisSol::ReceiverDB::getReceiver(size_t i) const {
  return receivers.at(i);
}

size_t SeisSol::ReceiverDB::numberOfReceivers() const { return receivers.size(); }
