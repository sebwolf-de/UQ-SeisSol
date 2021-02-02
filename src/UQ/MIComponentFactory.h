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

namespace UQ {

using namespace muq::Modeling;
using namespace muq::SamplingAlgorithms;
using namespace muq::Utilities;

class MyMIComponentFactory : public MIComponentFactory {
  public:
  virtual std::shared_ptr<MCMCProposal>
  Proposal(std::shared_ptr<MultiIndex> const& index,
           std::shared_ptr<AbstractSamplingProblem> const& samplingProblem) override;
  virtual std::shared_ptr<MultiIndex> FinestIndex() override;
  virtual std::shared_ptr<MCMCProposal>
  CoarseProposal(std::shared_ptr<MultiIndex> const& index,
                 std::shared_ptr<AbstractSamplingProblem> const& coarseProblem,
                 std::shared_ptr<SingleChainMCMC> const& coarseChain) override;
  virtual std::shared_ptr<AbstractSamplingProblem>
  SamplingProblem(std::shared_ptr<MultiIndex> const& index) override;
  virtual std::shared_ptr<MIInterpolation>
  Interpolation(std::shared_ptr<MultiIndex> const& index) override;
  virtual Eigen::VectorXd StartingPoint(std::shared_ptr<MultiIndex> const& index) override;

  MyMIComponentFactory(std::string filename);
};
} // namespace UQ
