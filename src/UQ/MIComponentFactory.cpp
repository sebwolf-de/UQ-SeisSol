#include "UQ/MIComponentFactory.h"

#include "MUQ/SamplingAlgorithms/MIComponentFactory.h"

#include "UQ/MIInterpolation.h"
#include "UQ/SamplingProblem.h"

#include "SeisSol/Runner.h"

std::shared_ptr<UQ::MCMCProposal> UQ::MyMIComponentFactory::Proposal(
    [[maybe_unused]] std::shared_ptr<MultiIndex> const& index,
    std::shared_ptr<AbstractSamplingProblem> const& samplingProblem) {
  pt::ptree pt;
  pt.put("BlockIndex", 0);

  size_t numberOfParameters = materialParameterWriter->numberOfParameters();

  auto mu = Eigen::VectorXd::Zero(numberOfParameters);
  Eigen::MatrixXd cov = Eigen::MatrixXd::Identity(numberOfParameters, numberOfParameters);
  
  for (size_t i=0; i < numberOfParameters; i++) {
    cov(i, i) = cov(i, i) * std::pow(startingParameters(i), 2) / 100;
  }

  auto prior = std::make_shared<Gaussian>(mu, cov, Gaussian::Mode::Covariance);

  return std::make_shared<MHProposal>(pt, samplingProblem, prior);
}

std::shared_ptr<UQ::MultiIndex> UQ::MyMIComponentFactory::FinestIndex() {
  auto index = std::make_shared<MultiIndex>(1);
  index->SetValue(0, 0);
  return index;
}

std::shared_ptr<UQ::MCMCProposal> UQ::MyMIComponentFactory::CoarseProposal(
    [[maybe_unused]] std::shared_ptr<MultiIndex> const& index,
    std::shared_ptr<AbstractSamplingProblem> const& coarseProblem,
    std::shared_ptr<SingleChainMCMC> const& coarseChain) {
  pt::ptree ptProposal;
  ptProposal.put("BlockIndex", 0);
  int subsampling = 5;
  ptProposal.put("Subsampling", subsampling);
  return std::make_shared<SubsamplingMIProposal>(ptProposal, coarseProblem, coarseChain);
}

std::shared_ptr<UQ::AbstractSamplingProblem>
UQ::MyMIComponentFactory::SamplingProblem(std::shared_ptr<MultiIndex> const& index) {
  return std::make_shared<MySamplingProblem>(index, runner, observationsReceiverDB,
                                             simulationsReceiverDB, materialParameterWriter, numberOfSubintervals);
}

std::shared_ptr<UQ::MIInterpolation> UQ::MyMIComponentFactory::Interpolation([
    [maybe_unused]] std::shared_ptr<MultiIndex> const& index) {
  return std::make_shared<MyInterpolation>();
}

Eigen::VectorXd UQ::MyMIComponentFactory::StartingPoint([
    [maybe_unused]] std::shared_ptr<MultiIndex> const& index) {
  return startingParameters;
}

UQ::MyMIComponentFactory::MyMIComponentFactory(
    std::shared_ptr<SeisSol::Runner> runner,
    std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
    std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB,
    std::shared_ptr<IO::MaterialParameterWriter> materialParameterWriter,
    const Eigen::VectorXd& startingParameters,
    size_t numberOfSubintervals)
    : runner(runner), observationsReceiverDB(observationsReceiverDB),
      simulationsReceiverDB(simulationsReceiverDB),
      materialParameterWriter(materialParameterWriter), startingParameters(startingParameters), numberOfSubintervals(numberOfSubintervals) {}
