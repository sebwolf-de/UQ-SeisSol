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

#include "SeisSol/Runner.h"

static constexpr int NUM_PARAM = 2;

namespace UQ {

  using namespace muq::Modeling;
  using namespace muq::SamplingAlgorithms;
  using namespace muq::Utilities;

  class MySamplingProblem : public AbstractSamplingProblem {
    public:
      MySamplingProblem(std::shared_ptr<MultiIndex> index, const SeisSol::Runner & runner);

      virtual ~MySamplingProblem(){};

      virtual double LogDensity(std::shared_ptr<SamplingState> const& state) override;
      virtual std::shared_ptr<SamplingState> QOI() override;

    private:
      const SeisSol::Runner & runner;
      std::shared_ptr<SamplingState> lastState = nullptr;
      std::shared_ptr<MultiIndex> index;
  };

} // namespace UQ
