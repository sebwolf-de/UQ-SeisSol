#include "ParameterReader.h"
#include "Eigen/src/Core/Matrix.h"
#include <cassert>
#include <yaml-cpp/emittermanip.h>

IO::ParameterReader::ParameterReader(std::string filename) : root(YAML::LoadFile(filename)) {
  assert((root["SeisSolBinary"] && "Parameter file contains link to SeisSol binary"));
  assert((root["ParametersFile"] && "Parameter file contains link to SeisSol parameters file"));
  assert((root["MaterialFileTemplate"] &&
          "Parameter file contains link to SeisSol material file template"));
  assert(
      (root["InitialParameters"] && "Parameter file contains initial material parameter values"));
  assert((root["NumberOfReceivers"] && "Parameter file contains number of receivers"));
  assert((root["NumberOfSamples"] && "Parameter file contains number of samples"));
  assert((root["NumberOfSubintervals"] && "Parameter file contains number of subintervals"));
  assert((root["ObservationDirectory"] && "Parameter file contains link to observation directory"));
}

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

size_t IO::ParameterReader::getNumberOfSamples(size_t index) const {
  const auto numberOfSamples = root["NumberOfSamples"].as<std::vector<int>>();
  return numberOfSamples.at(index);
}

size_t IO::ParameterReader::getNumberOfIndices() const {
  const auto numberOfSamples = root["NumberOfSamples"].as<std::vector<int>>();
  return numberOfSamples.size();
}

size_t IO::ParameterReader::getNumberOfSubintervals() const {
  return root["NumberOfSubintervals"].as<size_t>();
}

std::string IO::ParameterReader::getReceiverPrefix() const {
  if (root["ReceiverPrefix"]) {
    return root["ReceiverPrefix"].as<std::string>();
  } else {
    return "observation";
  }
}
