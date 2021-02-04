#pragma once

#include "Eigen/Dense"
#include <array>
#include <vector>

namespace SeisSol {
  class Receiver {
    public:
      //receiverData[i][0] contains the ith timestep
      //receiverData[i][j] contains the (j-1)th quantity at the ith timestep
      std::vector<std::array<double, 10>> receiverData;
      double l1Difference(const Receiver& otherData) const;
      Eigen::Vector3d position;
  };
};

