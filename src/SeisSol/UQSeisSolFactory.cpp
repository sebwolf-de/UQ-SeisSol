
#include "UQSeisSolFactory.h"

SeisSol::UQSeisSolFactory::UQSeisSolFactory(const IO::ParameterReader& parameterReader)
    : parameterReader(parameterReader) {}

std::shared_ptr<SeisSol::ReceiverDB>
SeisSol::UQSeisSolFactory::createObservationsReceiverDB() const {
  auto observationsReceiverDB = std::make_shared<SeisSol::ReceiverDB>(
      parameterReader.getObservationDir(), parameterReader.getReceiverPrefix());

  for (size_t i = 1; i < parameterReader.getNumberOfReceivers() + 1; i++) {
    observationsReceiverDB->addReceiver(i, 1);
  }

  return observationsReceiverDB;
}

std::shared_ptr<SeisSol::ReceiverDB>
SeisSol::UQSeisSolFactory::createSimulationsReceiverDB() const {
  return std::make_shared<SeisSol::ReceiverDB>("output/current",
                                               parameterReader.getReceiverPrefix(),
                                               parameterReader.getNumberOfFusedSims());
}

std::shared_ptr<IO::ChainParameterWriter>
SeisSol::UQSeisSolFactory::createChainParameterWriter() const {
  const std::string templateFilename = parameterReader.getChainFileTemplate();
  const std::ifstream chainFileTemplate(templateFilename);
  std::stringstream chainFileTemplateBuffer;
  chainFileTemplateBuffer << chainFileTemplate.rdbuf();

  std::string outputFilename = templateFilename;
  const std::string key = "template";
  const size_t locationOfKey = outputFilename.find(key);
  const size_t sizeOfKey = key.length();
  outputFilename.replace(locationOfKey, sizeOfKey, "chain");

  const auto parameterKeys = parameterReader.getChainFileTemplateKeys();

  return std::make_shared<IO::ChainParameterWriter>(chainFileTemplateBuffer.str(), outputFilename,
                                                    parameterKeys);
}

std::shared_ptr<SeisSol::Runner> SeisSol::UQSeisSolFactory::createSeisSolRunner() const {
  return std::make_shared<SeisSol::Runner>(parameterReader.getSeisSolBinary(),
                                           parameterReader.getMPICommand());
}
