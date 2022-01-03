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

#include <vector>
#include "boost/any.hpp"
#include <iostream>

#include "spdlog/spdlog.h"

#include <omp.h>

size_t UQ::MySamplingProblem::MySamplingProblem::runCount = 0;
// std::vector<Eigen::VectorXd> UQ::MySamplingProblem::MySamplingProblem::parameters;
std::vector<Eigen::VectorXd> parameters;

UQ::MySamplingProblem::MySamplingProblem(
    std::shared_ptr<MultiIndex> index, std::shared_ptr<SeisSol::Runner> runner,
    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
    std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter,
    size_t numberOfSubintervals,
    size_t numberOfFusedSims)
    : AbstractSamplingProblem(
          Eigen::VectorXi::Constant(1, materialParameterWriter->numberOfParameters()),
          Eigen::VectorXi::Constant(1, materialParameterWriter->numberOfParameters())),
      runner(runner), observationsReceiverDB(observationsReceiverDB),
      simulationsReceiverDB(simulationsReceiverDB),
      materialParameterWriter(materialParameterWriter), index(index), numberOfSubintervals(numberOfSubintervals), numberOfFusedSims(numberOfFusedSims) {
      spdlog::info("Run Sampling Problem with index {}", index->GetValue(0));
}

double UQ::MySamplingProblem::LogDensity(std::shared_ptr<SamplingState> const& state) {
  lastState = state;

  
  // spdlog::info("Running SeisSol on index {}", index->GetValue(0));
  // spdlog::debug("Running SeisSol with {} fused sims", numberOfFusedSims);
  // Eigen::VectorXd stateVector(numberOfFusedSims);
  // for (size_t i = 0; i < numberOfFusedSims; i++) {
  //   stateVector[i] = state->state[i][0];
  // }
  
  
  size_t fsn = omp_get_thread_num() + 1; // 1; // 
  
  #pragma omp single
  {
    parameters.resize(numberOfFusedSims);
    // std::cout << "Parametersize: " << parameters.size() << std::endl;
  }

  #pragma omp critical
  {
    parameters[omp_get_thread_num()] = state->state[0];
    // std::cout << "Thread: " << omp_get_thread_num() << std::endl;
  }

  #pragma omp barrier

  #pragma omp single
  {
    materialParameterWriter->updateParameters(parameters); //   state->state[0]
    spdlog::info("----------------------");
    spdlog::info("Running SeisSol on thread {}", omp_get_thread_num());
    runner->prepareFilesystem(runCount);
    runner->run(0); // index->GetValue(0)

    runCount++;
    spdlog::info("Executed SeisSol successfully {} times on thread: {}", runCount, omp_get_thread_num());
  }
  
  double relativeNorm = 0.0;
  const double epsilon = 1e-2;
  double logDensity;
  // double* logDensityArray = new double[numberOfFusedSims];

  // for (size_t fsn = 1; fsn <= numberOfFusedSims; fsn++) {
  std::vector<std::vector<double>> norm_diffs;
  std::vector<std::vector<double>> norms;
  size_t numberOfReceivers;

  #pragma omp critical // single copyprivate(norm_diffs, norms, numberOfReceivers) 
  {
    numberOfReceivers = observationsReceiverDB->numberOfReceivers(1);
    for (size_t i = 1; i < numberOfReceivers + 1; i++) {
      simulationsReceiverDB->addReceiver(i, fsn);
      
      norm_diffs.push_back(simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i), numberOfSubintervals, fsn));
      norms.push_back(observationsReceiverDB->getReceiver(i).l1Norm(numberOfSubintervals));
    }
  }

  relativeNorm = 0.0;
  #pragma omp critical
  {
    for (size_t i = 0; i < numberOfReceivers; i++) {
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
    spdlog::debug("Relative norm of receiver {}: {}, fused sim: {}", i, receiverRelativeNorm, fsn);
    }

    relativeNorm /= numberOfReceivers;
    logDensity = -std::pow(relativeNorm, 4); // logDensityArray[fsn-1] 
    spdlog::info("LogDensity {} = {}", fsn, logDensity); // logDensityArray[fsn-1]);
  }
  // }

  state->meta["LogTarget"] = logDensity; //logDensityArray;
  return logDensity; // logDensityArray[0];
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
