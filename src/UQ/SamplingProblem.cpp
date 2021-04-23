#include "UQ/SamplingProblem.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <sched.h>
#include <string>
#include <unistd.h>

size_t UQ::MySamplingProblem::MySamplingProblem::runCount = 0;

UQ::MySamplingProblem::MySamplingProblem(
    std::shared_ptr<MultiIndex> index, std::shared_ptr<SeisSol::Runner> runner,
    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
    std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter)
    : AbstractSamplingProblem(
          Eigen::VectorXi::Constant(1, materialParameterWriter->numberOfParameters()),
          Eigen::VectorXi::Constant(1, materialParameterWriter->numberOfParameters())),
      runner(runner), observationsReceiverDB(observationsReceiverDB),
      simulationsReceiverDB(simulationsReceiverDB),
      materialParameterWriter(materialParameterWriter), index(index) {
  std::cout << "Run Sampling Problem with index" << index->GetValue(0) << std::endl;
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  lastState = state;

  //discard unphysical values
  if (state->state[0].minCoeff() < 0) {
    return -40;
  }

  std::cout << "----------------------" << std::endl;
  std::cout << "Run SeisSol on index " << index->GetValue(0) << std::endl;

  materialParameterWriter->updateParameters(state->state[0]);

  runner->prepareFilesystem(runCount);
  runner->run(index->GetValue(0));

  std::cout << "Executed SeisSol successfully " << ++runCount << " times." << std::endl;

  double norm_diff = 0.0;
  double norm = 0.0;

  for (size_t i = 1; i < observationsReceiverDB->numberOfReceivers() + 1; i++) {
    simulationsReceiverDB->addReceiver(i);
    norm_diff += simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i));
    norm += observationsReceiverDB->getReceiver(i).l1Norm();
  }

  const auto relative_norm = norm_diff/norm;
  const auto logDensity = -std::pow(relative_norm, 4);
  std::cout << "LogDensity = " << logDensity << std::endl;
  std::cout << std::endl;
  return logDensity;
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
