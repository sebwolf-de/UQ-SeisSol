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
  std::cout << "Size of state: " << state->state.size() << std::endl;
  std::cout << "Size of state: " << state->state[0].size() << std::endl;
  materialParameterWriter->updateParameters(state->state, numberOfFusedSims);
  spdlog::info("----------------------");
  spdlog::info("Running SeisSol on index {}", index->GetValue(0));
  runner->prepareFilesystem(runCount);
  runner->run(0);

  runCount++;
  spdlog::info("Executed SeisSol successfully {} times", runCount);
  
  
  double relativeNorm = 0.0;
  const double epsilon = 1e-2;
  double* logDensityArray = new double[numberOfFusedSims];

  for (size_t fsn = 1; fsn <= numberOfFusedSims; fsn++) {


    std::vector<std::vector<double>> norm_diffs;
    std::vector<std::vector<double>> norms;

    for (size_t i = 1; i < observationsReceiverDB->numberOfReceivers(1) + 1; i++) {
      simulationsReceiverDB->addReceiver(i, fsn);
      
      norm_diffs.push_back(simulationsReceiverDB->l1Difference(i, observationsReceiverDB->getReceiver(i), numberOfSubintervals, fsn));
      norms.push_back(observationsReceiverDB->getReceiver(i).l1Norm(numberOfSubintervals));
    }
    

    relativeNorm = 0.0;
    
    for (size_t i = 0; i < observationsReceiverDB->numberOfReceivers(1); i++) {
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

    relativeNorm /= observationsReceiverDB->numberOfReceivers(1);
    logDensityArray[fsn-1] = -std::pow(relativeNorm, 4); 
    spdlog::info("LogDensity {} = {}", fsn, logDensityArray[fsn-1]); 
  }

  state->meta["LogTarget"] = logDensityArray; 
  return logDensityArray[0]; 
}

std::shared_ptr<UQ::SamplingState> UQ::MySamplingProblem::QOI() {
  assert(lastState != nullptr);
  return std::make_shared<SamplingState>(lastState->state, 1.0);
}
