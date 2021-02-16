#include "UQ/SamplingProblem.h"

UQ::MySamplingProblem::MySamplingProblem(
  std::shared_ptr<MultiIndex> index,
  std::shared_ptr<SeisSol::Runner> runner,
  std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
  std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB
) :
  AbstractSamplingProblem(Eigen::VectorXi::Constant(1, NUM_PARAM),
  Eigen::VectorXi::Constant(1, NUM_PARAM)),
  runner(runner),
  observationsReceiverDB(observationsReceiverDB),
  simulationsReceiverDB(simulationsReceiverDB),
  index(index) {
    std::cout << "Run Sampling Problem with index" << index->GetValue(0) << std::endl;
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  lastState = state;

  // Modify material_chain.yaml file with the candidate material parameters given in state

  runner->run();

  double norm = 0.0;

  for (size_t i = 1; i < observationsReceiverDB->numberOfReceivers() + 1; i++) {
    simulationsReceiverDB->addReceiver(i);
    norm += simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i));
  }

  return norm;
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
