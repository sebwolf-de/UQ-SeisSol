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

  size_t numReceivers = parameterReader.getReceiverNumber();
  std::cout << "Reading " << numReceivers << " receivers" << std::endl;
  
  SeisSol::ReceiverDB receiverDB(parameterReader.getObservationDir(), parameterReader.getPrefix());
  for (size_t i = 1; i < numReceivers+1; i++) {
    receiverDB.addReceiver(i);
  }

  //Just for Debug purposes
  std::cout << "L1 difference between the 1st and the last read receivers: " << std::endl;
  const auto& someReceiver = receiverDB.getReceiver(numReceivers);
  std::cout << receiverDB.l1Difference(1, someReceiver) << std::endl;


  auto runner = std::make_shared<SeisSol::Runner>(parameterReader.getSeisSolBinary());

  auto localFactory = std::make_shared<UQ::MyMIComponentFactory>(runner);

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
