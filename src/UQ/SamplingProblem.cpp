#include "UQ/SamplingProblem.h"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <cmath>
#include <functional>
#include <iterator>
#include <numeric>
#include <sched.h>
#include <string>
#include <unistd.h>

#include "boost/any.hpp"
#include <iostream>
#include <vector>

#include "spdlog/spdlog.h"

size_t UQ::MySamplingProblem::MySamplingProblem::runCount = 0;

UQ::MySamplingProblem::MySamplingProblem(
    std::shared_ptr<MultiIndex> index, std::shared_ptr<SeisSol::Runner> runner,
    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
    std::shared_ptr<IO::ChainParameterWriter> chainParameterWriter, size_t numberOfSubintervals,
    size_t numberOfFusedSims, std::shared_ptr<muq::Modeling::Gaussian> targetIn)
    : AbstractSamplingProblem(
          Eigen::VectorXi::Constant(1, chainParameterWriter->numberOfParameters()),
          Eigen::VectorXi::Constant(1, chainParameterWriter->numberOfParameters())),
      runner(std::move(runner)), observationsReceiverDB(std::move(observationsReceiverDB)),
      simulationsReceiverDB(std::move(simulationsReceiverDB)),
      chainParameterWriter(std::move(chainParameterWriter)), index(index),
      numberOfSubintervals(numberOfSubintervals), numberOfFusedSims(numberOfFusedSims),
      target(std::move(targetIn)) {
  spdlog::info("Run Sampling Problem with index {}", index->GetValue(0));
}

Eigen::VectorXd UQ::MySamplingProblem::GradLogDensity(std::shared_ptr<SamplingState> const& state,
                                                      [[maybe_unused]] unsigned blockWrt) {
  return target->GradLogDensity(0, state->state); // 0 instead of wrt
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  lastState = state;

  double relativeNorm = 0.0;
  const double epsilon = 1e-2;
  std::vector<double> logDensityArray(numberOfFusedSims);

  size_t numOutOfMesh = 0;
  for (size_t fsn = 1; fsn <= numberOfFusedSims; fsn++) {
    if (state->state[fsn - 1][0] < 0) {
      logDensityArray.at(fsn - 1) = badLogDensity;
      numOutOfMesh++;
    }
  }

  if (numOutOfMesh == numberOfFusedSims) {
    spdlog::info("######################");
    spdlog::info("Skipping SeisSol on index {}, since all sources are outside of the mesh", 
        index->GetValue(0));

    state->meta["LogTarget"] = logDensityArray;
    return logDensityArray.at(0);
  }

  spdlog::info("######################");
  spdlog::info("Running SeisSol on index {}", index->GetValue(0));
  spdlog::info("----------------------");
  chainParameterWriter->updateParameters(state->state);
  spdlog::info("----------------------");
  runner->prepareFilesystem(runCount);
  runner->run(0);

  runCount++;
  spdlog::info("Executed SeisSol successfully {} times", runCount);

  for (size_t fsn = 1; fsn <= numberOfFusedSims; fsn++) {

    if (state->state[fsn - 1][0] < 0) {
      continue;
    }

    std::vector<std::vector<double>> normDiffs;
    std::vector<std::vector<double>> norms;

    for (size_t i = 1; i < observationsReceiverDB->numberOfReceivers(1) + 1; i++) {
      simulationsReceiverDB->addReceiver(i, fsn);

      normDiffs.push_back(simulationsReceiverDB->l1Difference(
          i, observationsReceiverDB->getReceiver(i), numberOfSubintervals, fsn));
      norms.push_back(observationsReceiverDB->getReceiver(i).l1Norm(numberOfSubintervals));
    }

    relativeNorm = 0.0;

    for (size_t i = 0; i < observationsReceiverDB->numberOfReceivers(1); i++) {
      double receiverRelativeNorm = 0.0;
      for (size_t j = 0; j < normDiffs[i].size(); j++) {
        if (norms[i][j] > epsilon) {
          receiverRelativeNorm += normDiffs[i][j] / norms[i][j];
        } else {
          receiverRelativeNorm += normDiffs[i][j];
        }
      }
      receiverRelativeNorm = receiverRelativeNorm / (double)numberOfSubintervals;
      relativeNorm += receiverRelativeNorm;
      spdlog::debug("Relative norm of receiver {}: {}, fused sim: {}", i, receiverRelativeNorm,
                    fsn);
    }

    relativeNorm /= observationsReceiverDB->numberOfReceivers(1);
    logDensityArray.at(fsn - 1) = -std::pow(relativeNorm, 4);
  }
  for (size_t fsn = 1; fsn <= numberOfFusedSims; fsn++) {
    if (state->state[fsn - 1][0] < 0) {
      spdlog::info("LogDensity {} = {}, source out of mesh", fsn, logDensityArray[fsn - 1]);
    } else {
      spdlog::info("LogDensity {} = {}", fsn, logDensityArray[fsn - 1]);
    }
  }

  state->meta["LogTarget"] = logDensityArray;
  return logDensityArray.at(0);
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
