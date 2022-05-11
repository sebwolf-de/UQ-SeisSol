#include "spdlog/spdlog.h"

#include "MaterialParameterWriter.h"

IO::MaterialParameterWriter::MaterialParameterWriter(const std::string& templateFileContent,
                                                     const std::string& outputFilename,
                                                     const std::vector<std::string>& parameterKeys)
    : templateFileContent(templateFileContent), outputFilename(outputFilename),
      parameterKeys(parameterKeys) {}

void IO::MaterialParameterWriter::updateParameters(Eigen::VectorXd parameters) const {
  std::string unsavedFileContent = templateFileContent;

  for (size_t i = 0; i < parameterKeys.size(); i++) {
    std::string key = parameterKeys[i];

    size_t locationOfKey = unsavedFileContent.find("@" + key + "@");

    unsavedFileContent.replace(locationOfKey, key.length() + 2, std::to_string(parameters(i)));
    spdlog::info("{:<25s}: {:e}", key, parameters(i));
  }

  std::ofstream materialParametersFile(outputFilename);

  materialParametersFile << unsavedFileContent;
  materialParametersFile.close();
}

void IO::MaterialParameterWriter::updateParameters(std::vector<Eigen::VectorXd> parameters) const {
  std::string unsavedFileContent = templateFileContent;
  for (size_t j = 0; j < parameters.size(); j++) {
    for (size_t i = 0; i < parameterKeys.size(); i++) {
      std::string key = parameterKeys[i] + "_" + std::to_string(j + 1);

      size_t locationOfKey = unsavedFileContent.find("@" + key + "@");

      unsavedFileContent.replace(locationOfKey, key.length() + 2, std::to_string(parameters[j](i)));
      spdlog::info("{:<25s}: {:e}: {}", key, parameters[j](i), j);
    }
  }

  std::ofstream materialParametersFile(outputFilename);

  materialParametersFile << unsavedFileContent;
  materialParametersFile.close();
}

size_t IO::MaterialParameterWriter::numberOfParameters() const { return parameterKeys.size(); }
