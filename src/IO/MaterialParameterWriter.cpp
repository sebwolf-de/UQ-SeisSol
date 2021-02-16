
#include "MaterialParameterWriter.h"

IO::MaterialParameterWriter::MaterialParameterWriter(
  std::string templateFileContent,
  size_t numberOfParameters,
  std::map<int, std::string> parameterKeys
) :
  templateFileContent(templateFileContent),
  unsavedContent(templateFileContent),
  numberOfParameters(numberOfParameters),
  parameterKeys(parameterKeys) {}

void IO::MaterialParameterWriter::updateParameters(Eigen::VectorXd parameters) {
  for (size_t i = 0; i < numberOfParameters; i++) {
    std::string key = parameterKeys.at(i);

    size_t locationOfKey = unsavedContent.find("@" + key + "@");

    unsavedContent.replace(
      locationOfKey,
      key.length() + 2,
      std::to_string(parameters(i))
    );
  }
}

void IO::MaterialParameterWriter::save() {
  std::ofstream materialParametersFile("material_chain.yaml");
  
  materialParametersFile << unsavedContent;
  materialParametersFile.close();

  unsavedContent = templateFileContent;
}
