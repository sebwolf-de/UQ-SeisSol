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

int main(int argc, char** argv) {

  auto localFactory = std::make_shared<UQ::MyMIComponentFactory>("true_solution.dat");

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
