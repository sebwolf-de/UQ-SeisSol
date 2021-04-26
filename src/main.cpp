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

#include "IO/ParameterReader.h"
#include "SeisSol/UQSeisSolFactory.h"

int main(int argc, char** argv) {
  assert(argc == 2);

  auto parameterReader = IO::ParameterReader(argv[1]);
  auto uqSeisSolFactory = SeisSol::UQSeisSolFactory(parameterReader);

  auto observationsReceiverDB = uqSeisSolFactory.createObservationsReceiverDB();
  auto simulationsReceiverDB = uqSeisSolFactory.createSimulationsReceiverDB();
  auto materialParameterWriter = uqSeisSolFactory.createMaterialParameterWriter();
  auto runner = uqSeisSolFactory.createSeisSolRunner();

  auto initialParameterValues = parameterReader.getInitialMaterialParameterValues();

  auto miComponentFactory = std::make_shared<UQ::MyMIComponentFactory>(
      runner, observationsReceiverDB, simulationsReceiverDB, materialParameterWriter,
      initialParameterValues, parameterReader.getNumberOfIndices()-1);

  boost::property_tree::ptree pt;
  pt.put("verbosity", 1); // show some output
  pt.put("BurnIn", 1);
  for (size_t i = 0; i < parameterReader.getNumberOfIndices(); i++) {
    char buffer[12];
    sprintf(buffer, "NumSamples_%i", i);
    pt.put(buffer, parameterReader.getNumberOfSamples(i));
  }

  muq::SamplingAlgorithms::MIMCMC mimcmc(pt, miComponentFactory);
  std::shared_ptr<muq::SamplingAlgorithms::SampleCollection> samples = mimcmc.Run();

  std::cout << "ML mean Param: " << mimcmc.MeanParam().transpose() << std::endl;
  //std::cout << "ML mean QOI: " << mimcmc.MeanQOI().transpose() << std::endl;
  mimcmc.WriteToFile("test.h5");

  return 0;
}
