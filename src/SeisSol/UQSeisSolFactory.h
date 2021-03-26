#pragma once

#include "IO/MaterialParameterWriter.h"
#include "IO/ParameterReader.h"
#include "ReceiverDB.h"
#include "Runner.h"

namespace SeisSol {

class UQSeisSolFactory {
  public:
  UQSeisSolFactory(const IO::ParameterReader& parameterReader);

  SeisSol::ReceiverDB createObservationsReceiverDB() const;
  SeisSol::ReceiverDB createSimulationsReceiverDB() const;
  IO::MaterialParameterWriter createMaterialParameterWriter() const;
  SeisSol::Runner createSeisSolRunner() const;

  private:
  const IO::ParameterReader& parameterReader;
};

} // namespace SeisSol
