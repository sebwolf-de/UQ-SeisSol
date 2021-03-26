
#include "MaterialParameterWriter.h"

IO::MaterialParameterWriter::MaterialParameterWriter(std::string templateFileContent,
                                                     std::vector<std::string> parameterKeys)
    : templateFileContent(std::move(templateFileContent)), parameterKeys(std::move(parameterKeys)) {}

void IO::MaterialParameterWriter::updateParameters(Eigen::VectorXd parameters) const {
  std::string unsavedFileContent = templateFileContent;

  for (size_t i = 0; i < parameterKeys.size(); i++) {
    std::string key = parameterKeys[i];

    size_t locationOfKey = unsavedFileContent.find("@" + key + "@");

    unsavedFileContent.replace(locationOfKey, key.length() + 2, std::to_string(parameters(i)));
  }

  std::ofstream materialParametersFile("material_chain.yaml");

  materialParametersFile << unsavedFileContent;
  materialParametersFile.close();
}

size_t IO::MaterialParameterWriter::numberOfParameters() const { return parameterKeys.size(); }
