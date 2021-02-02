#include "UQ/SamplingProblem.h"

UQ::MySamplingProblem::MySamplingProblem(std::shared_ptr<MultiIndex> index)
    : AbstractSamplingProblem(Eigen::VectorXi::Constant(1, NUM_PARAM),
                              Eigen::VectorXi::Constant(1, NUM_PARAM)) {
  this->index = index;
  std::cout << "Run Sampling Problem with index" << index->GetValue(0) << std::endl;
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  // Call SeisSol
  return 0.0;
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
