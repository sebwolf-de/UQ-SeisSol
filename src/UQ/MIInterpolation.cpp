#include "UQ/MIInterpolation.h"

std::shared_ptr<UQ::SamplingState>
UQ::MyInterpolation::Interpolate(std::shared_ptr<SamplingState> const& coarseProposal,
                                 std::shared_ptr<SamplingState> const& fineProposal) {
  return std::make_shared<SamplingState>(coarseProposal->state);
}
