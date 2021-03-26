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
  MySamplingProblem(const std::shared_ptr<MultiIndex>& index, SeisSol::Runner& runner,
                    SeisSol::ReceiverDB& observationsReceiverDB,
                    SeisSol::ReceiverDB& simulationsReceiverDB,
                    const IO::MaterialParameterWriter& materialParameterWriter);

  virtual ~MySamplingProblem(){};

  virtual double LogDensity(std::shared_ptr<SamplingState> const& state) override;
  virtual std::shared_ptr<SamplingState> QOI() override;

  private:
  SeisSol::Runner& runner;
  SeisSol::ReceiverDB& observationsReceiverDB;
  SeisSol::ReceiverDB& simulationsReceiverDB;
  const IO::MaterialParameterWriter& materialParameterWriter;
  std::shared_ptr<SamplingState> lastState = nullptr;
  const std::shared_ptr<MultiIndex>& index;
};

} // namespace UQ
