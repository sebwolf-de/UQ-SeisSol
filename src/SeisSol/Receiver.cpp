#include "Receiver.h"

double SeisSol::Receiver::l1Difference(const Receiver& otherReceiver) const {
  assert(receiverData.size() == otherReceiver.receiverData.size());

  double l = 0;
  
  for (size_t i = 0; i < receiverData.size(); i++) {
    for (size_t j = 1; j < 10; j++) {
      //TODO think of something better here
      //proably we need time interpolation once we have data read from SeisSol output. We also need to weight different quantities (stresses are ~1e9, velocities ~1e0)
      l += std::abs(receiverData[i][j] - otherReceiver.receiverData[i][j]);
    }
  }
  
  return l;
};
