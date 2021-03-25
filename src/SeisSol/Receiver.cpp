#include "Receiver.h"

SeisSol::Receiver::Receiver() : relevantQuantities({1,2,3}) {}

double SeisSol::Receiver::l1Difference(const Receiver& otherReceiver) const {
  assert(receiverData.size() == otherReceiver.receiverData.size());

  double l = 0;

  for (size_t i = 0; i < receiverData.size(); i++) {
    for (size_t j : relevantQuantities) {
      l += std::abs(this->operator()(i, j) - otherReceiver(i, j));
    }
  }

  return l;
}

double SeisSol::Receiver::l1Norm() const {
  double l = 0;

  for (size_t i = 0; i < receiverData.size(); i++) {
    for (size_t j : relevantQuantities) {
      l += std::abs(this->operator()(i, j));
    }
  }

  return l;
}

void SeisSol::Receiver::clear() { receiverData.clear(); }

void SeisSol::Receiver::appendData(std::array<double, 10> data) { receiverData.emplace_back(data); }

double SeisSol::Receiver::operator()(size_t t, size_t q) const {
  assert(t < receiverData.size());
  assert(q < 10);
  return receiverData[t][q];
}
