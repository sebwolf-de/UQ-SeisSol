#pragma once

#include <Eigen/Dense>
#include <cstring>
#include <yaml-cpp/yaml.h>

namespace IO {

class ParameterReader {
  public:
  ParameterReader(std::string filename);
  std::string getSeisSolBinary() const;
  std::string getMaterialFileTemplate() const;
  std::vector<std::string> getMaterialFileTemplateKeys() const;
  Eigen::VectorXd getInitialMaterialParameterValues() const;
  std::string getObservationDir() const;
  size_t getNumberOfReceivers() const;
  size_t getNumberOfSamples(size_t index) const;
  size_t getNumberOfIndices() const;
  size_t getNumberOfSubintervals() const;
  std::string getReceiverPrefix() const;
  size_t getNumberOfFusedSims() const;

  private:
  const YAML::Node root;
};

} // namespace IO
