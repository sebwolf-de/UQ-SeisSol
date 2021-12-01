#include "FusedReceiver.h"
#include <vector>
#include <cmath>

SeisSol::FusedReceiver::FusedReceiver(size_t numberOfFusedSims = 1) : relevantQuantities({7,8,9}), numberOfFusedSims(numberOfFusedSims) {
    receiverData = std::array<std::vector<std::array<double, 10>>, numberOfFusedSims>;
}


void SeisSol::Receiver::appendData(std::array<double, 10> data, size_t fusedSimulationNumber = 1) { receiverData[fusedSimulationNumber].emplace_back(data); }

double SeisSol::Receiver::operator()(size_t t, size_t q, size_t fsn = 1) const {
  assert(t < receiverData[fsn].size());
  assert(q < 10);
  return receiverData[fsn][t][q];
}

std::vector<double> SeisSol::Receiver::l1Difference(const Receiver& otherReceiver, size_t numberOfSubintervals, size_t fsn = 1) const {
  assert(receiverData[fsn].size() == otherReceiver.receiverData.size());

  std::vector<double> diffs;
  
  size_t subintervalSize = std::floor(receiverData[fsn].size() / numberOfSubintervals);
  //assume constant dt
  const double dt = this->operator()(1,0) - this->operator()(0,0);

  for (size_t s = 0; s < numberOfSubintervals; s++) {
    double l = 0.0;

    size_t start = s * subintervalSize;
    size_t end = (start + subintervalSize > receiverData[fsn].size()) ? receiverData[fsn].size() : start + subintervalSize;

    for (size_t i = start; i < end; i++) {
      for (size_t j : relevantQuantities) {
        l += std::abs(this->operator()(i, j) - otherReceiver(i, j));
      }
    }

    diffs.push_back(dt * l);
  }

  return diffs;
}

std::vector<double> SeisSol::Receiver::l1Norm(size_t numberOfSubintervals, size_t fsn = 1) const {
  std::vector<double> norms;
  
  const size_t subintervalSize = std::floor(receiverData[fsn].size() / numberOfSubintervals);
  //assume constant dt
  const double dt = this->operator()(1,0) - this->operator()(0,0);

  for (size_t s = 0; s < numberOfSubintervals; s++) {
    double l = 0.0;

    size_t start = s * subintervalSize;
    size_t end = (start + subintervalSize > receiverData[fsn].size()) ? receiverData[fsn].size() : start + subintervalSize;

    for (size_t i = start; i < end; i++) {
      for (size_t j : relevantQuantities) {
        l += std::abs(this->operator()(i, j));
      }
    }

    norms.push_back(dt * l);
  }

  return norms;
}
