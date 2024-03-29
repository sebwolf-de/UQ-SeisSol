#include "UQ/SamplingProblem.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <sched.h>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <cmath>

#include "spdlog/spdlog.h"

size_t UQ::MySamplingProblem::MySamplingProblem::runCount = 0;

UQ::MySamplingProblem::MySamplingProblem(
    std::shared_ptr<MultiIndex> index, std::shared_ptr<SeisSol::Runner> runner,
    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
    std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter,
    size_t numberOfSubintervals)
    : AbstractSamplingProblem(
          Eigen::VectorXi::Constant(1, materialParameterWriter->numberOfParameters()),
          Eigen::VectorXi::Constant(1, materialParameterWriter->numberOfParameters())),
      runner(runner), observationsReceiverDB(observationsReceiverDB),
      simulationsReceiverDB(simulationsReceiverDB),
      materialParameterWriter(materialParameterWriter), index(index), numberOfSubintervals(numberOfSubintervals) {
      spdlog::info("Run Sampling Problem with index {}", index->GetValue(0));
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  lastState = state;

  //discard unphysical values
  //if (state->state[0].minCoeff() < 0) {
  //  return -40;
  //}

  spdlog::info("----------------------");
  spdlog::info("Running SeisSol on index {}", index->GetValue(0));

  materialParameterWriter->updateParameters(state->state[0]);

  runner->prepareFilesystem(runCount);
  runner->run(index->GetValue(0));

  runCount++;
  spdlog::info("Executed SeisSol successfully {} times", runCount);

  std::vector<std::vector<double>> norm_diffs;
  std::vector<std::vector<double>> norms;

  for (size_t i = 1; i < observationsReceiverDB->numberOfReceivers() + 1; i++) {
    simulationsReceiverDB->addReceiver(i);

    norm_diffs.push_back(simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i), numberOfSubintervals));
    norms.push_back(observationsReceiverDB->getReceiver(i).l1Norm(numberOfSubintervals));
  }

  double relativeNorm = 0.0;
  const double epsilon = 1e-2;

  for (size_t i = 0; i < observationsReceiverDB->numberOfReceivers(); i++) {
    double receiverRelativeNorm = 0.0;
    for (size_t j = 0; j < norm_diffs[i].size(); j++) {
      if (norms[i][j] > epsilon) {
        receiverRelativeNorm += norm_diffs[i][j] / norms[i][j];
      }
      else {
        receiverRelativeNorm += norm_diffs[i][j];
      }
    }
    receiverRelativeNorm = receiverRelativeNorm / (double)numberOfSubintervals;
    relativeNorm += receiverRelativeNorm;
    spdlog::debug("Relative norm of receiver {}: {}", i, receiverRelativeNorm);
  }
  relativeNorm /= observationsReceiverDB->numberOfReceivers();

  const auto logDensity = -std::pow(relativeNorm-2, 4);
  spdlog::info("LogDensity = {}", logDensity);
  return logDensity;
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
