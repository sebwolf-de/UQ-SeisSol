#pragma once

#include "Eigen/Dense"
#include <array>
#include <vector>
#include "Receiver.h"

namespace SeisSol {
class FusedReceiver : Receiver {
    public: 
    FusedReceiver(size_t numberOfFusedSims);
    std::vector<double> l1Difference(const Receiver& otherData, size_t numberOfSubintervals, size_t fsn) const;   
    std::vector<double> l1Norm(size_t numberOfSubintervals, size_t fsn) const;
    void appendData(std::array<double, 10>, size_t fusedSimulationNumber);
    double operator()(size_t t, size_t q, size_t fsn) const;

    private:
    size_t numberOfFusedSims;
    std::array<std::vector<std::array<double, 10>>, numberOfFusedSims> receiverData;
};

} // namespace SeisSol