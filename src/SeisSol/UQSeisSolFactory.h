#pragma once

#include "IO/MaterialParameterWriter.h"
#include "IO/ParameterReader.h"
#include "ReceiverDB.h"
#include "Runner.h"

namespace SeisSol {

class UQSeisSolFactory {
  public:
  UQSeisSolFactory(const IO::ParameterReader& parameterReader);

  [[nodiscard]] std::shared_ptr<SeisSol::ReceiverDB> createObservationsReceiverDB() const;
  [[nodiscard]] std::shared_ptr<SeisSol::ReceiverDB> createSimulationsReceiverDB() const;
  [[nodiscard]] std::shared_ptr<IO::MaterialParameterWriter> createMaterialParameterWriter() const;
  [[nodiscard]] std::shared_ptr<SeisSol::Runner> createSeisSolRunner() const;

  private:
  const IO::ParameterReader& parameterReader;
};

} // namespace SeisSol
