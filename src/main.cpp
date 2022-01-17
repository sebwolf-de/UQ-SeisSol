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

#include "spdlog/spdlog.h"

int main(int argc, char** argv) {
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

  boost::property_tree::ptree pt;
  pt.put("verbosity", 1); // show some output
  pt.put("BurnIn", 1);
  for (size_t i = 0; i < parameterReader.getNumberOfIndices(); i++) {
    char buffer_num[13];
    sprintf(buffer_num, "NumSamples_%lu", i);
    pt.put(buffer_num, parameterReader.getNumberOfSamples(i));
    spdlog::info("Use {} samples on level {}", parameterReader.getNumberOfSamples(i), i);
  }


  
  muq::SamplingAlgorithms::MIMCMC mimcmc(pt, miComponentFactory);
  std::shared_ptr<muq::SamplingAlgorithms::SampleCollection> samples = mimcmc.Run();

  std::stringstream output_mean;
  output_mean << "Thread: " << omp_get_thread_num() << "; ML mean Param: " << mimcmc.MeanParam().transpose();
  spdlog::info(output_mean.str());
  //std::stringstream output_qoi;
  //output_qoi << "ML qoi Param: " << mimcmc.MeanQOI().transpose();
  //spdlog::info(output_qoi.str());

  mimcmc.WriteToFile("test.h5");
  
  
  return 0;
}
