#include "MUQ/SamplingAlgorithms/DummyKernel.h"
#include "MUQ/SamplingAlgorithms/GreedyMLMCMC.h"
#include "MUQ/SamplingAlgorithms/MIMCMC.h"
#include "MUQ/SamplingAlgorithms/ParallelFixedSamplesMIMCMC.h"
#include "MUQ/SamplingAlgorithms/ParallelMIComponentFactory.h"
#include "MUQ/SamplingAlgorithms/SLMCMC.h"
#include <boost/property_tree/ptree.hpp>

#include "UQ/MIComponentFactory.h"
#include "UQ/MIInterpolation.h"
#include "UQ/SamplingProblem.h"

#include "SeisSol/Runner.h"
#include "Reader/ParameterReader.h"
#include "Reader/ReceiverReader.h"
#include "SeisSol/ReceiverDB.h"

int main(int argc, char** argv) {
  assert(argc == 2);

  auto parameterReader = Reader::ParameterReader(argv[1]);

  size_t numberOfReceivers = parameterReader.getNumberOfReceivers();
  std::cout << "Reading " << numberOfReceivers << " receivers" << std::endl;
  
  auto observationsReceiverDB = std::make_shared<SeisSol::ReceiverDB>(
    parameterReader.getObservationDir(),
    parameterReader.getReceiverPrefix()
  );

  for (size_t i = 1; i < numberOfReceivers+1; i++) {
    observationsReceiverDB->addReceiver(i);
  }

  auto simulationsReceiverDB = std::make_shared<SeisSol::ReceiverDB>("output", parameterReader.getReceiverPrefix());

  auto runner = std::make_shared<SeisSol::Runner>(parameterReader.getSeisSolBinary(), parameterReader.getParametersFile());

  auto localFactory = std::make_shared<UQ::MyMIComponentFactory>(runner, observationsReceiverDB, simulationsReceiverDB);

  boost::property_tree::ptree pt;
  const size_t N = 1e4;
  // pt.put("NumSamples", N); // number of samples for single level
  pt.put("verbosity", 1); // show some output
  pt.put("BurnIn", 10);
  pt.put("NumSamples_0", 1000);
  pt.put("NumSamples_1", 100);
  pt.put("NumSamples_2", 10);

  muq::SamplingAlgorithms::MIMCMC mimcmc(pt, localFactory);
  std::shared_ptr<muq::SamplingAlgorithms::SampleCollection> samples = mimcmc.Run();

  std::cout << "ML mean Param: " << mimcmc.MeanParam().transpose() << std::endl;
  std::cout << "ML mean QOI: " << mimcmc.MeanQOI().transpose() << std::endl;
  mimcmc.WriteToFile("test.hdf5");

  return 0;
}
