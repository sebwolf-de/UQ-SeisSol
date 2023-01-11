#include "spdlog/spdlog.h"

#include "ChainParameterWriter.h"

IO::ChainParameterWriter::ChainParameterWriter(const std::string templateFileContent,
                                               const std::string outputFilename,
                                               const std::vector<std::string> parameterKeys)
    : templateFileContent(std::move(templateFileContent)),
      outputFilename(std::move(outputFilename)), parameterKeys(std::move(parameterKeys)) {}

void IO::ChainParameterWriter::updateParameters(Eigen::VectorXd parameters) const {
  std::string unsavedFileContent = templateFileContent;

  for (size_t i = 0; i < parameterKeys.size(); i++) {
    const std::string key = parameterKeys[i];

    const size_t locationOfKey = unsavedFileContent.find("@" + key + "@");

    unsavedFileContent.replace(locationOfKey, key.length() + 2, std::to_string(parameters(i)));
    spdlog::info("{:<25s}: {:e}", key, parameters(i));
  }

  std::ofstream chainParametersFile(outputFilename);

  chainParametersFile << unsavedFileContent;
  chainParametersFile.close();
}

void IO::ChainParameterWriter::updateParameters(std::vector<Eigen::VectorXd> parameters) const {
  std::string unsavedFileContent = templateFileContent;
  for (size_t j = 0; j < parameters.size(); j++) {
    for (size_t i = 0; i < parameterKeys.size(); i++) {
      const std::string key = parameterKeys[i] + "_" + std::to_string(j + 1);

      const size_t locationOfKey = unsavedFileContent.find("@" + key + "@");

      unsavedFileContent.replace(locationOfKey, key.length() + 2, std::to_string(parameters[j](i)));
      spdlog::info("{:<25s}: {:e}: {}", key, parameters[j](i), j);
    }
  }

  std::ofstream chainParametersFile(outputFilename);

  chainParametersFile << unsavedFileContent;
  chainParametersFile.close();
}

size_t IO::ChainParameterWriter::numberOfParameters() const { return parameterKeys.size(); }
