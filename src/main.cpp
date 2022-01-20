#include "MUQ/SamplingAlgorithms/SamplingProblem.h"
#include "MUQ/SamplingAlgorithms/SingleChainMCMC.h"

#include "MUQ/SamplingAlgorithms/GMHKernel.h"
// #include "MUQ/SamplingAlgorithms/FusedGMHKernel.h"
#include "MUQ/SamplingAlgorithms/TransitionKernel.h"
#include "MUQ/SamplingAlgorithms/SampleCollection.h"
#include "MUQ/Utilities/MultiIndices/MultiIndex.h"


#include <boost/property_tree/ptree.hpp>

#include "UQ/MIComponentFactory.h"
#include "UQ/MIInterpolation.h"
#include "UQ/SamplingProblem.h"

#include "IO/ParameterReader.h"
#include "SeisSol/UQSeisSolFactory.h"

#include "spdlog/spdlog.h"

#include <iostream>

using namespace muq::Modeling;
using namespace muq::SamplingAlgorithms;
using namespace muq::Utilities;


int main(int argc, char** argv){
  assert(argc == 2);
  spdlog::set_level(spdlog::level::debug); // Set global log level to debug

  auto parameterReader = IO::ParameterReader(argv[1]);
  auto uqSeisSolFactory = SeisSol::UQSeisSolFactory(parameterReader);

  auto observationsReceiverDB = uqSeisSolFactory.createObservationsReceiverDB();
  auto simulationsReceiverDB = uqSeisSolFactory.createSimulationsReceiverDB();
  auto materialParameterWriter = uqSeisSolFactory.createMaterialParameterWriter();
  auto runner = uqSeisSolFactory.createSeisSolRunner();

  runner->archivePreviousRun();

  auto initialParameterValues = parameterReader.getInitialMaterialParameterValues();
  size_t numberOfSubintervals = parameterReader.getNumberOfSubintervals();
  size_t numberOfFusedSims    = parameterReader.getNumberOfFusedSims();

  auto miComponentFactory = std::make_shared<UQ::MyMIComponentFactory>(
      runner, observationsReceiverDB, simulationsReceiverDB, materialParameterWriter,
      initialParameterValues, parameterReader.getNumberOfIndices()-1, numberOfSubintervals, numberOfFusedSims);


  auto index = std::make_shared<MultiIndex>(1);
  index->SetValue(0, 0);
  auto problem = miComponentFactory->SamplingProblem(index);
  auto proposal = miComponentFactory->Proposal(index, problem);

  // parameters for the sampler
  boost::property_tree::ptree pt;
  pt.put("verbosity", 1); // show some output
  pt.put("BurnIn", 1);
  pt.put("NumSamples", parameterReader.getNumberOfSamples(0));
  pt.put("PrintLevel", 0);
  // for (size_t i = 0; i < parameterReader.getNumberOfIndices(); i++) {
  //   char buffer_num[13];
  //   sprintf(buffer_num, "NumSamples_%lu", i);
  //   pt.put(buffer_num, parameterReader.getNumberOfSamples(i));
  //   spdlog::info("Use {} samples on level {}", parameterReader.getNumberOfSamples(i), i);
  // }

  const unsigned int N = numberOfFusedSims;
  const unsigned int M = numberOfFusedSims;
  pt.put("NumProposals", N);
  pt.put("NumAccepted", M); // optional: defaults to N
  std::cout << "Num of N: " << N << std::endl;

  std::vector<std::shared_ptr<TransitionKernel>> kernels(1);
  kernels[0] = std::make_shared<GMHKernel>(pt, problem, proposal);

  auto chain = std::make_shared<SingleChainMCMC>(pt, kernels);
  chain->SetState(initialParameterValues);
  std::shared_ptr<SampleCollection> samps = chain->Run();

  std::cout << "\nSample Mean = \n" << samps->Mean().transpose() << std::endl;

  samps->WriteToFile("test.h5");

  return 0;
}
