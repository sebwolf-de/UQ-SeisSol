
#include "UQSeisSolFactory.h"

SeisSol::UQSeisSolFactory::UQSeisSolFactory(const IO::ParameterReader& parameterReader)
    : parameterReader(parameterReader){}

std::shared_ptr<SeisSol::ReceiverDB>
SeisSol::UQSeisSolFactory::createObservationsReceiverDB() const {
  auto observationsReceiverDB = std::make_shared<SeisSol::ReceiverDB>(
      parameterReader.getObservationDir(), parameterReader.getReceiverPrefix());

  for (size_t i = 1; i < parameterReader.getNumberOfReceivers() + 1; i++) {
    observationsReceiverDB->addReceiver(i);
  }

  return observationsReceiverDB;
}

std::shared_ptr<SeisSol::ReceiverDB>
SeisSol::UQSeisSolFactory::createSimulationsReceiverDB() const {
  return std::make_shared<SeisSol::ReceiverDB>("output", parameterReader.getReceiverPrefix());
}

std::shared_ptr<IO::MaterialParameterWriter>
SeisSol::UQSeisSolFactory::createMaterialParameterWriter() const {
  std::ifstream materialFileTemplate(parameterReader.getMaterialFileTemplate());
  std::stringstream materialFileTemplateBuffer;
  materialFileTemplateBuffer << materialFileTemplate.rdbuf();

  auto parameterKeys = parameterReader.getMaterialFileTemplateKeys();

  return std::make_shared<IO::MaterialParameterWriter>(materialFileTemplateBuffer.str(),
                                                       parameterKeys);
}

std::shared_ptr<SeisSol::Runner> SeisSol::UQSeisSolFactory::createSeisSolRunner() const {
  return std::make_shared<SeisSol::Runner>(parameterReader.getSeisSolBinary(),
                                           parameterReader.getParametersFile());
}
