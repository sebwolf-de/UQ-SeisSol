#pragma once

#include <Eigen/Dense>
#include <cstring>
#include <yaml-cpp/yaml.h>

namespace IO {

struct ValuesAndVariances {
  Eigen::VectorXd values;
  Eigen::VectorXd variances;
};

class ParameterReader {
  public:
  ParameterReader(std::string filename);
  std::string getSeisSolBinary() const;
  std::string getMaterialFileTemplate() const;
  std::vector<std::string> getMaterialFileTemplateKeys() const;
  ValuesAndVariances getInitialMaterialParameterValuesAndVariance() const;
  std::string getObservationDir() const;
  size_t getNumberOfReceivers() const;
  size_t getNumberOfSamples(size_t index) const;
  size_t getNumberOfIndices() const;
  size_t getNumberOfSubintervals() const;
  std::string getReceiverPrefix() const;

  private:
  const YAML::Node root;
};

} // namespace IO
