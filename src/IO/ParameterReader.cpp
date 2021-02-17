#include "ParameterReader.h"
#include "Eigen/src/Core/Matrix.h"
#include <cassert>
#include <iostream>
#include <yaml-cpp/emittermanip.h>

IO::ParameterReader::ParameterReader(std::string filename) : root(YAML::LoadFile(filename)) {
  assert(("Parameter file contains link to SeisSol binary", root["SeisSolBinary"]));
  assert(("Parameter file contains link to SeisSol parameters file", root["ParametersFile"]));
  assert(("Parameter file contains link to SeisSol material file template",
          root["MaterialFileTemplate"]));
  assert(("Parameter file contains initial material parameter values", root["InitialParameters"]));
  assert(("Parameter file contains number of receivers", root["NumberOfReceivers"]));
  assert(("Parameter file contains link to observation directory", root["ObservationDirectory"]));
};

std::string IO::ParameterReader::getSeisSolBinary() const {
  return root["SeisSolBinary"].as<std::string>();
}

std::string IO::ParameterReader::getParametersFile() const {
  return root["ParametersFile"].as<std::string>();
}

std::string IO::ParameterReader::getMaterialFileTemplate() const {
  return root["MaterialFileTemplate"].as<std::string>();
}

std::vector<std::string> IO::ParameterReader::getMaterialFileTemplateKeys() const {
  auto parametersNode = root["InitialParameters"];
  std::vector<std::string> parameterKeys;

  for (YAML::const_iterator iter = parametersNode.begin(); iter != parametersNode.end(); ++iter) {
    parameterKeys.push_back(iter->first.as<std::string>());
  }

  return parameterKeys;
}

Eigen::VectorXd IO::ParameterReader::getInitialMaterialParameterValues() const {
  auto parametersNode = root["InitialParameters"];
  Eigen::VectorXd parameterValues(parametersNode.size());

  size_t i = 0;

  for (YAML::const_iterator iter = parametersNode.begin(); iter != parametersNode.end(); ++iter) {
    parameterValues(i++) = iter->second.as<double>();
  }

  return parameterValues;
}

std::string IO::ParameterReader::getObservationDir() const {
  return root["ObservationDirectory"].as<std::string>();
}

size_t IO::ParameterReader::getNumberOfReceivers() const {
  return root["NumberOfReceivers"].as<size_t>();
}

std::string IO::ParameterReader::getReceiverPrefix() const {
  if (root["ReceiverPrefix"]) {
    return root["ReceiverPrefix"].as<std::string>();
  } else {
    return "observation";
  }
}
