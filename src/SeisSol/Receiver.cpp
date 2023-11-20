#include "Receiver.h"
#include <cmath>
#include <vector>

#include <iostream>

SeisSol::Receiver::Receiver() : relevantQuantities({uIndex, vIndex, wIndex}) {}

std::vector<double> SeisSol::Receiver::l1Difference(const Receiver& otherReceiver,
                                                    size_t numberOfSubintervals) const {
  if (receiverData.size() != otherReceiver.receiverData.size()) {
    std::cout << "receiverData.size(): " << receiverData.size() << std::endl;
    std::cout << "otherReceiver.receiverData.size(): " << otherReceiver.receiverData.size()
              << std::endl;
  }
  assert(receiverData.size() == otherReceiver.receiverData.size());

  std::vector<double> diffs;

  const size_t subintervalSize = std::floor(receiverData.size() / numberOfSubintervals);
  // assume constant dt
  const double dt = this->operator()(1, 0) - this->operator()(0, 0);

  for (size_t s = 0; s < numberOfSubintervals; s++) {
    double l = 0.0;

    const size_t start = s * subintervalSize;
    const size_t end = (start + subintervalSize > receiverData.size()) ? receiverData.size()
                                                                       : start + subintervalSize;

    for (size_t i = start; i < end; i++) {
      for (const size_t j : relevantQuantities) {
        l += std::abs(this->operator()(i, j) - otherReceiver(i, j));
      }
    }

    diffs.push_back(dt * l);
  }

  return diffs;
}

std::vector<double> SeisSol::Receiver::l1Norm(size_t numberOfSubintervals) const {
  std::vector<double> norms;

  const size_t subintervalSize = std::floor(receiverData.size() / numberOfSubintervals);
  // assume constant dt
  const double dt = this->operator()(1, 0) - this->operator()(0, 0);

  for (size_t s = 0; s < numberOfSubintervals; s++) {
    double l = 0.0;

    const size_t start = s * subintervalSize;
    const size_t end = (start + subintervalSize > receiverData.size()) ? receiverData.size()
                                                                       : start + subintervalSize;
    for (size_t i = start; i < end; i++) {
      for (const size_t j : relevantQuantities) {
        l += std::abs(this->operator()(i, j));
      }
    }

    norms.push_back(dt * l);
  }

  return norms;
}

void SeisSol::Receiver::clear() { receiverData.clear(); }

void SeisSol::Receiver::appendData(std::array<double, lineWidth> data) {
  receiverData.emplace_back(data);
}

double SeisSol::Receiver::operator()(size_t t, size_t q) const {
  assert(t < receiverData.size());
  assert(q < 10);
  return receiverData[t][q];
}
