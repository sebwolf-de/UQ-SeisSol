#include "UQ/MIComponentFactory.h"

#include "MUQ/SamplingAlgorithms/MIComponentFactory.h"

#include "UQ/MIInterpolation.h"
#include "UQ/SamplingProblem.h"

#include "SeisSol/Runner.h"

std::shared_ptr<UQ::MCMCProposal> UQ::MyMIComponentFactory::Proposal(
    std::shared_ptr<MultiIndex> const& index,
    std::shared_ptr<AbstractSamplingProblem> const& samplingProblem) {
  pt::ptree pt;
  pt.put("BlockIndex", 0);

  auto mu = Eigen::VectorXd::Zero(NUM_PARAM);
  Eigen::MatrixXd cov = Eigen::MatrixXd::Identity(NUM_PARAM, NUM_PARAM);
  cov *= 0.005;

  auto prior = std::make_shared<Gaussian>(mu, cov);

  return std::make_shared<MHProposal>(pt, samplingProblem, prior);
}

std::shared_ptr<UQ::MultiIndex> UQ::MyMIComponentFactory::FinestIndex() {
  auto index = std::make_shared<MultiIndex>(1);
  index->SetValue(0, 2);
  return index;
}

std::shared_ptr<UQ::MCMCProposal> UQ::MyMIComponentFactory::CoarseProposal(
  std::shared_ptr<MultiIndex> const& index,
  std::shared_ptr<AbstractSamplingProblem> const& coarseProblem,
  std::shared_ptr<SingleChainMCMC> const& coarseChain
) {
  pt::ptree ptProposal;
  ptProposal.put("BlockIndex", 0);
  int subsampling = 5;
  ptProposal.put("Subsampling", subsampling);
  return std::make_shared<SubsamplingMIProposal>(ptProposal, coarseProblem, coarseChain);
}

std::shared_ptr<UQ::AbstractSamplingProblem> UQ::MyMIComponentFactory::SamplingProblem(
  std::shared_ptr<MultiIndex> const& index
) {
  return std::make_shared<MySamplingProblem>(index, runner, observationsReceiverDB, simulationsReceiverDB);
}

std::shared_ptr<UQ::MIInterpolation>
UQ::MyMIComponentFactory::Interpolation(std::shared_ptr<MultiIndex> const& index) {
  return std::make_shared<MyInterpolation>();
}

Eigen::VectorXd UQ::MyMIComponentFactory::StartingPoint(std::shared_ptr<MultiIndex> const& index) {
  Eigen::VectorXd start = Eigen::VectorXd::Ones(NUM_PARAM);
  start(0) = .5;
  start(1) = .5;
  return start;
}

UQ::MyMIComponentFactory::MyMIComponentFactory(
  std::shared_ptr<SeisSol::Runner> runner,
  std::shared_ptr<SeisSol::ReceiverDB> observationsReceiverDB,
  std::shared_ptr<SeisSol::ReceiverDB> simulationsReceiverDB
) :
  runner(runner),
  observationsReceiverDB(observationsReceiverDB),
  simulationsReceiverDB(simulationsReceiverDB) {}
