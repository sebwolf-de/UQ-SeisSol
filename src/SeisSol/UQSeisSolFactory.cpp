
#include "UQSeisSolFactory.h"

SeisSol::UQSeisSolFactory::UQSeisSolFactory(const IO::ParameterReader& parameterReader)
    : parameterReader(parameterReader) {}

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
  return std::make_shared<SeisSol::ReceiverDB>("output/current", parameterReader.getReceiverPrefix(), parameterReader.getNumberOfFusedSims);
}

std::shared_ptr<IO::MaterialParameterWriter>
SeisSol::UQSeisSolFactory::createMaterialParameterWriter() const {
  std::string templateFilename = parameterReader.getMaterialFileTemplate();
  std::ifstream materialFileTemplate(templateFilename);
  std::stringstream materialFileTemplateBuffer;
  materialFileTemplateBuffer << materialFileTemplate.rdbuf();

  std::string outputFilename = templateFilename;
  size_t locationOfKey = outputFilename.find("template");
  outputFilename.replace(locationOfKey, 8, "chain");

  auto parameterKeys = parameterReader.getMaterialFileTemplateKeys();

  return std::make_shared<IO::MaterialParameterWriter>(materialFileTemplateBuffer.str(),
                                                       outputFilename,
                                                       parameterKeys);
}

std::shared_ptr<SeisSol::Runner> SeisSol::UQSeisSolFactory::createSeisSolRunner() const {
  return std::make_shared<SeisSol::Runner>(parameterReader.getSeisSolBinary());
}
