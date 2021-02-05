#include "ReceiverList.h"
#include "SeisSol/Receiver.h"

SeisSol::ReceiverList::ReceiverList(std::string directory, std::string prefix) :
  receiverReader(Reader::ReceiverReader(directory, prefix)) {};

void SeisSol::ReceiverList::addReceiver(size_t i) {
  SeisSol::Receiver receiver;
  receiverReader.parseReceiver(i, receiver);
  receivers.insert({i, receiver});
};

double SeisSol::ReceiverList::l1Difference(size_t i, const SeisSol::Receiver& otherReceiver) const {
  return receivers.at(i).l1Difference(otherReceiver);
};

const SeisSol::Receiver& SeisSol::ReceiverList::getReceiver(size_t i) const {
  return receivers.at(i);
}

