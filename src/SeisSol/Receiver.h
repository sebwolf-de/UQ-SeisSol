#pragma once

#include "Eigen/Dense"
#include <array>
#include <vector>

namespace SeisSol {
class Receiver {
  public:
  double l1Difference(const Receiver& otherData) const;
  Eigen::Vector3d position;
  void clear();
  void appendData(std::array<double, 10>);
  double operator()(size_t t, size_t q) const;

  private:
  // receiverData[i][0] contains the ith timestep
  // receiverData[i][j] contains the (j-1)th quantity at the ith timestep
  std::vector<std::array<double, 10>> receiverData;
};
}; // namespace SeisSol
