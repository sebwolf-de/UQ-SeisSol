#pragma once

#include "Eigen/Dense"
#include <array>
#include <vector>

namespace SeisSol {
class Receiver {
  public:
  constexpr static size_t uIndex = 7;
  constexpr static size_t vIndex = 8;
  constexpr static size_t wIndex = 9;
  constexpr static size_t numberOfQuantities = 9;
  constexpr static size_t lineWidth = 10;
  Receiver();
  [[nodiscard]] std::vector<double> l1Difference(const Receiver& otherReceiver,
                                                 size_t numberOfSubintervals) const;
  [[nodiscard]] std::vector<double> l1Norm(size_t numberOfSubintervals) const;
  void clear();
  void appendData(std::array<double, lineWidth> data);
  [[nodiscard]] double operator()(size_t t, size_t q) const;

  private:
  Eigen::Vector3d position;
  // receiverData[i][0] contains the ith timestep
  // receiverData[i][j] contains the (j-1)th quantity at the ith timestep
  std::vector<size_t> relevantQuantities;
  std::vector<std::array<double, lineWidth>> receiverData;
};
} // namespace SeisSol
