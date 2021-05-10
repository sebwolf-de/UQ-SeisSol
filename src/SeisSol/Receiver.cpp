#include "Receiver.h"
#include <vector>
#include <cmath>

SeisSol::Receiver::Receiver() : relevantQuantities({7,8,9}) {}

std::vector<double> SeisSol::Receiver::l1Difference(const Receiver& otherReceiver, size_t numberOfSubintervals) const {
  assert(receiverData.size() == otherReceiver.receiverData.size());

  std::vector<double> diffs;
  
  size_t subintervalSize = std::floor(receiverData.size() / numberOfSubintervals);

  for (size_t s = 0; s < numberOfSubintervals; s++) {
    double l = 0.0;

    size_t start = s * subintervalSize;
    size_t end = (start + subintervalSize > receiverData.size()) ? receiverData.size() : start + subintervalSize;

    for (size_t i = start; i < end; i++) {
      for (size_t j : relevantQuantities) {
        l += std::abs(this->operator()(i, j) - otherReceiver(i, j));
      }
    }

    diffs.push_back(l);
  }

  return diffs;
}

std::vector<double> SeisSol::Receiver::l1Norm(size_t numberOfSubintervals) const {
  std::vector<double> norms;
  
  size_t subintervalSize = std::floor(receiverData.size() / numberOfSubintervals);

  for (size_t s = 0; s < numberOfSubintervals; s++) {
    double l = 0.0;

    size_t start = s * subintervalSize;
    size_t end = (start + subintervalSize > receiverData.size()) ? receiverData.size() : start + subintervalSize;

    for (size_t i = start; i < end; i++) {
      for (size_t j : relevantQuantities) {
        l += std::abs(this->operator()(i, j));
      }
    }

    norms.push_back(l);
  }

  return norms;
}

void SeisSol::Receiver::clear() { receiverData.clear(); }

void SeisSol::Receiver::appendData(std::array<double, 10> data) { receiverData.emplace_back(data); }

double SeisSol::Receiver::operator()(size_t t, size_t q) const {
  assert(t < receiverData.size());
  assert(q < 10);
  return receiverData[t][q];
}
