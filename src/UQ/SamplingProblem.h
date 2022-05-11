#pragma once

#include "MUQ/Modeling/Distributions/Density.h"
#include "MUQ/Modeling/Distributions/Gaussian.h"
#include "MUQ/SamplingAlgorithms/CrankNicolsonProposal.h"
#include "MUQ/SamplingAlgorithms/MHProposal.h"
#include "MUQ/SamplingAlgorithms/MIComponentFactory.h"
#include "MUQ/SamplingAlgorithms/MIInterpolation.h"
#include "MUQ/SamplingAlgorithms/SamplingProblem.h"
#include "MUQ/SamplingAlgorithms/SubsamplingMIProposal.h"
#include "MUQ/Utilities/MultiIndices/MultiIndex.h"

#include "IO/MaterialParameterWriter.h"
#include "SeisSol/ReceiverDB.h"
#include "SeisSol/Runner.h"

namespace UQ {

using namespace muq::Modeling;
using namespace muq::SamplingAlgorithms;
using namespace muq::Utilities;

class MySamplingProblem : public AbstractSamplingProblem {
  public:
  constexpr static double badLogDensity = -1200000;
  MySamplingProblem(std::shared_ptr<MultiIndex> index, std::shared_ptr<SeisSol::Runner> runner,
                    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
                    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
                    std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter,
                    size_t numberOfSubintervals, size_t numberOfFusedSims,
                    std::shared_ptr<muq::Modeling::Gaussian> targetIn);

  ~MySamplingProblem() override = default;

  double LogDensity(std::shared_ptr<SamplingState> const& state) override;
  std::shared_ptr<SamplingState> QOI() override;
  // Needed for MALAProposal:
  Eigen::VectorXd GradLogDensity(std::shared_ptr<SamplingState> const& state,
                                 unsigned blockWrt) override;

  private:
  std::shared_ptr<SeisSol::Runner> runner;
  std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB;
  std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB;
  std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter;
  std::shared_ptr<SamplingState> lastState = nullptr;
  std::shared_ptr<MultiIndex> index;
  static size_t runCount;

  size_t numberOfSubintervals;
  size_t numberOfFusedSims;

  /// The target distribution (the prior in the inference case)
  std::shared_ptr<muq::Modeling::Gaussian> target;
};

} // namespace UQ
