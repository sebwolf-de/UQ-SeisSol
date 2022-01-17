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
  
  size_t fsn = omp_get_thread_num() + 1;
  
  
  parameters.resize(numberOfFusedSims);
  // std::cout << "Parametersize: " << parameters.size() << std::endl;
  parameters[omp_get_thread_num()] = state->state[0];
  // std::cout << "Thread: " << omp_get_thread_num() << std::endl;
  

  materialParameterWriter->updateParameters(parameters); //   state->state[0]
  spdlog::info("----------------------");
  spdlog::info("Running SeisSol");
  runner->prepareFilesystem(runCount);
  runner->run(0); // index->GetValue(0)

  runCount++;
  spdlog::info("Executed SeisSol successfully {} times", runCount);
  
  
  double relativeNorm = 0.0;
  const double epsilon = 1e-2;
  double logDensity;

  std::vector<std::vector<double>> norm_diffs;
  std::vector<std::vector<double>> norms;
  size_t numberOfReceivers;

  
  numberOfReceivers = observationsReceiverDB->numberOfReceivers(1);
  for (size_t i = 1; i < numberOfReceivers + 1; i++) {
    simulationsReceiverDB->addReceiver(i, fsn);
    
    norm_diffs.push_back(simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i), numberOfSubintervals, fsn));
    norms.push_back(observationsReceiverDB->getReceiver(i).l1Norm(numberOfSubintervals));
  }
  

  relativeNorm = 0.0;
  
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
  logDensity = -std::pow(relativeNorm, 4); 
  spdlog::info("LogDensity {} = {}", fsn, logDensity); 
  

  state->meta["LogTarget"] = logDensity; 
  return logDensity; 
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
