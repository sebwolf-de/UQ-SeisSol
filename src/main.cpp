#include "MUQ/SamplingAlgorithms/SamplingProblem.h"
#include "MUQ/SamplingAlgorithms/SingleChainMCMC.h"

#include "MUQ/SamplingAlgorithms/FusedGMHKernel.h"
#include "MUQ/SamplingAlgorithms/SampleCollection.h"
#include "MUQ/SamplingAlgorithms/TransitionKernel.h"
#include "MUQ/Utilities/MultiIndices/MultiIndex.h"

#include <boost/property_tree/ptree.hpp>

#include "UQ/MIComponentFactory.h"
#include "UQ/MIInterpolation.h"
#include "UQ/SamplingProblem.h"

#include "IO/ParameterReader.h"
#include "SeisSol/UQSeisSolFactory.h"

#include "spdlog/spdlog.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

#include <mpi.h>

using namespace muq::Modeling;
using namespace muq::SamplingAlgorithms;
using namespace muq::Utilities;

int main(int argc, char* argv[]) {
  assert(argc == 2);
  MPI_Init(&argc, &argv);
  spdlog::set_level(spdlog::level::debug); // Set global log level to debug

  const auto parameterFileName = std::string(argv[1]);
  auto parameterReader = IO::ParameterReader(parameterFileName);
  auto uqSeisSolFactory = SeisSol::UQSeisSolFactory(parameterReader);

  auto observationsReceiverDB = uqSeisSolFactory.createObservationsReceiverDB();
  auto simulationsReceiverDB = uqSeisSolFactory.createSimulationsReceiverDB();
  auto chainParameterWriter = uqSeisSolFactory.createChainParameterWriter();
  auto runner = uqSeisSolFactory.createSeisSolRunner();

  runner->archivePreviousRun();

  auto initialParameterValuesAndVariance =
      parameterReader.getInitialChainParameterValuesAndVariance();
  const size_t numberOfSubintervals = parameterReader.getNumberOfSubintervals();
  const size_t numberOfFusedSims = parameterReader.getNumberOfFusedSims();

  auto miComponentFactory = std::make_shared<UQ::MyMIComponentFactory>(
      runner, observationsReceiverDB, simulationsReceiverDB, chainParameterWriter,
      initialParameterValuesAndVariance, parameterReader.getNumberOfIndices() - 1,
      numberOfSubintervals, numberOfFusedSims);

  auto index = std::make_shared<MultiIndex>(1);
  index->SetValue(0, 0);
  auto problem = miComponentFactory->SamplingProblem(index);
  auto proposal = miComponentFactory->Proposal(index, problem);

  // parameters for the sampler
  boost::property_tree::ptree pt;
  pt.put("verbosity", 1); // show some output
  pt.put("BurnIn", numberOfFusedSims);
  pt.put("NumSamples", parameterReader.getNumberOfSamples(0));
  pt.put("PrintLevel", 1);

  const unsigned int numberOfProposals = numberOfFusedSims;
  const unsigned int numberOfAcceptedProposals = numberOfFusedSims; // /2;
  pt.put("NumProposals", numberOfProposals);
  pt.put("NumAccepted", numberOfAcceptedProposals);
  // pt.put("StepSize", 0.5);

  std::vector<std::shared_ptr<TransitionKernel>> kernels(1);
  kernels[0] = std::make_shared<FusedGMHKernel>(pt, problem, proposal);

  auto chain = std::make_shared<SingleChainMCMC>(pt, kernels);
  chain->SetState(initialParameterValuesAndVariance.values);
  const std::shared_ptr<SampleCollection> samps = chain->Run();

  samps->WriteToFile("test.h5");
  std::cout << "Sample Mean = " << samps->Mean().transpose() << std::endl;
  std::cout << "Variance = " << samps->Variance().transpose() << std::endl;
  std::cout << "ESS = " << samps->ESS().transpose() << std::endl;
  std::cout << "Finished all" << std::endl;

  MPI_Finalize();
  return 0;
}
