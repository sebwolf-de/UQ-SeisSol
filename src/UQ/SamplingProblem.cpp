#include "UQ/SamplingProblem.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <sched.h>
#include <string>

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
      materialParameterWriter(materialParameterWriter), index(index), runCount(0) {
  std::cout << "Run Sampling Problem with index" << index->GetValue(0) << std::endl;
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  lastState = state;
  std::cout << "----------------------" << std::endl;
  std::cout << "Run SeisSol on index " << index->GetValue(0) << std::endl;

  materialParameterWriter->updateParameters(state->state[0]);

  // Copies the old receiver data to output/chain_xx and removes them from output/current
  std::string chain = "output/chain";
  if (!boost::filesystem::exists(chain)) {
    boost::filesystem::create_directory(chain);
  }
  std::string current = "output/current";
  const auto receiverList = IO::getReceiversInDirectory(current, "output-receiver");
  for (auto it = receiverList.begin(); it != receiverList.end(); it++) {
    boost::filesystem::copy_file(it->second, chain + "/receiver-" + std::to_string(it->first) + "-chain-" + std::to_string(runCount) + ".dat");
  }
  boost::filesystem::remove_all(current);
  boost::filesystem::create_directory(current);

  runner->run();
  std::cout << "Executed SeisSol successfully " << ++runCount << " times." << std::endl;

  double norm_diff = 0.0;
  double norm = 0.0;

  for (size_t i = 1; i < observationsReceiverDB->numberOfReceivers() + 1; i++) {
    simulationsReceiverDB->addReceiver(i);
    norm_diff += simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i));
    norm += observationsReceiverDB->getReceiver(i).l1Norm();
  }

  const auto relative_norm = norm_diff/norm;
  std::cout << "LogDensity = " << -relative_norm << std::endl;
  std::cout << std::endl;
  return -relative_norm;
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
