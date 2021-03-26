#include "UQSeisSolFactory.h"

SeisSol::UQSeisSolFactory::UQSeisSolFactory(const IO::ParameterReader& parameterReader)
    : parameterReader(parameterReader) {}

SeisSol::ReceiverDB
SeisSol::UQSeisSolFactory::createObservationsReceiverDB() const {
  auto observationsReceiverDB = SeisSol::ReceiverDB(
      parameterReader.getObservationDir(), parameterReader.getReceiverPrefix());

  for (size_t i = 1; i < parameterReader.getNumberOfReceivers() + 1; i++) {
    observationsReceiverDB.addReceiver(i);
  }

  return observationsReceiverDB;
}

SeisSol::ReceiverDB
SeisSol::UQSeisSolFactory::createSimulationsReceiverDB() const {
  return SeisSol::ReceiverDB("output", parameterReader.getReceiverPrefix());
}

IO::MaterialParameterWriter SeisSol::UQSeisSolFactory::createMaterialParameterWriter() const {
  std::ifstream materialFileTemplate(parameterReader.getMaterialFileTemplate());
  std::stringstream materialFileTemplateBuffer;
  materialFileTemplateBuffer << materialFileTemplate.rdbuf();

  auto parameterKeys = parameterReader.getMaterialFileTemplateKeys();

  return IO::MaterialParameterWriter(materialFileTemplateBuffer.str(),
                                                       parameterKeys);
}

SeisSol::Runner SeisSol::UQSeisSolFactory::createSeisSolRunner() const {
  return SeisSol::Runner(parameterReader.getSeisSolBinary(),
                                           parameterReader.getParametersFile(),
                                           parameterReader.getNumberOfProcesses());
}
