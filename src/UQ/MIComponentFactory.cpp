#include "UQ/MIComponentFactory.h"

#include "MUQ/SamplingAlgorithms/MIComponentFactory.h"


#include "UQ/MIInterpolation.h"
#include "UQ/SamplingProblem.h"

#include "SeisSol/Runner.h"


#include "MUQ/SamplingAlgorithms/MALAProposal.h"
#include "MUQ/SamplingAlgorithms/InfMALAProposal.h"

std::shared_ptr<UQ::MCMCProposal> UQ::MyMIComponentFactory::Proposal(
    [[maybe_unused]] std::shared_ptr<MultiIndex> const& index,
    std::shared_ptr<AbstractSamplingProblem> const& samplingProblem) {
  pt::ptree pt;
  pt.put("BlockIndex", 0);

  size_t numberOfParameters = materialParameterWriter->numberOfParameters();

  auto mu = Eigen::VectorXd::Zero(numberOfParameters);
  Eigen::MatrixXd cov = Eigen::MatrixXd::Identity(numberOfParameters, numberOfParameters);
  // Eigen::VectorXd cov = Eigen::VectorXd::Ones(numberOfParameters);
  
  for (size_t i=0; i < numberOfParameters; i++) {
    cov(i, i) = startingParameters.variances(i); // cov(i, i)
  }

  auto prior = std::make_shared<Gaussian>(mu, cov, Gaussian::Mode::Covariance);

  return std::make_shared<InfMALAProposal>(pt, samplingProblem, prior); //   InfMALAProposal MHProposal
}

std::shared_ptr<UQ::MultiIndex> UQ::MyMIComponentFactory::FinestIndex() {
  auto index = std::make_shared<MultiIndex>(1);
  index->SetValue(0, finestIndex);
  return index;
}

std::shared_ptr<UQ::MCMCProposal> UQ::MyMIComponentFactory::CoarseProposal(
    [[maybe_unused]] std::shared_ptr<MultiIndex> const& fineIndex,
    [[maybe_unused]] std::shared_ptr<MultiIndex> const& coarseIndex,
    std::shared_ptr<AbstractSamplingProblem> const& coarseProblem,
    std::shared_ptr<SingleChainMCMC> const& coarseChain) {
  pt::ptree ptProposal;
  ptProposal.put("BlockIndex", 0);
  int subsampling = 5;
  ptProposal.put("MLMCMC.Subsampling_0", subsampling);
  return std::make_shared<SubsamplingMIProposal>(ptProposal, coarseProblem, coarseIndex, coarseChain);
}

std::shared_ptr<UQ::AbstractSamplingProblem>
UQ::MyMIComponentFactory::SamplingProblem(std::shared_ptr<MultiIndex> const& index) {
  size_t numberOfParameters = materialParameterWriter->numberOfParameters();

  auto mu = Eigen::VectorXd::Zero(numberOfParameters);
  Eigen::MatrixXd cov = Eigen::MatrixXd::Identity(numberOfParameters, numberOfParameters);
  // Eigen::VectorXd cov = Eigen::VectorXd::Ones(numberOfParameters);

  for (size_t i=0; i < numberOfParameters; i++) {
    cov(i, i) = startingParameters.variances(i); // cov(i, i)
  }

  auto prior = std::make_shared<Gaussian>(mu, cov, Gaussian::Mode::Covariance);
  return std::make_shared<MySamplingProblem>(index, runner, observationsReceiverDB,
                                             simulationsReceiverDB, materialParameterWriter, numberOfSubintervals, numberOfFusedSims, prior);
}

std::shared_ptr<UQ::MIInterpolation> UQ::MyMIComponentFactory::Interpolation([
    [maybe_unused]] std::shared_ptr<MultiIndex> const& index) {
  return std::make_shared<MyInterpolation>();
}

Eigen::VectorXd UQ::MyMIComponentFactory::StartingPoint([
    [maybe_unused]] std::shared_ptr<MultiIndex> const& index) {
  return startingParameters.values;
}

UQ::MyMIComponentFactory::MyMIComponentFactory(
    std::shared_ptr<SeisSol::Runner> runner,
    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
    std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter,
    const IO::ValuesAndVariances& startingParameters,
    size_t finestIndex,
    size_t numberOfSubintervals,
    size_t numberOfFusedSims)
    : runner(runner), observationsReceiverDB(observationsReceiverDB),
      simulationsReceiverDB(simulationsReceiverDB),
      materialParameterWriter(materialParameterWriter), startingParameters(startingParameters),
      finestIndex(finestIndex), numberOfSubintervals(numberOfSubintervals), numberOfFusedSims(numberOfFusedSims) {}
