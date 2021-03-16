#pragma once

#include <Eigen/Dense>
#include <cstring>
#include <yaml-cpp/yaml.h>

namespace IO {

class ParameterReader {
  public:
  ParameterReader(std::string filename);
  std::string getSeisSolBinary() const;
  std::string getParametersFile() const;
  std::string getNumberOfProcesses() const;
  std::string getMaterialFileTemplate() const;
  std::vector<std::string> getMaterialFileTemplateKeys() const;
  Eigen::VectorXd getInitialMaterialParameterValues() const;
  std::string getObservationDir() const;
  size_t getNumberOfReceivers() const;
  std::string getReceiverPrefix() const;

  private:
  const YAML::Node root;
};

} // namespace IO
