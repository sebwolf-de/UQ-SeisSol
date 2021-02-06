#pragma once

#include <yaml-cpp/yaml.h>
#include <cstring>
#include <Eigen/Dense>

namespace Reader{
  class ParameterReader {
    public:
      ParameterReader(std::string filename);
      std::string getSeisSolBinary() const;
      std::string getObservationDir() const;
      size_t getNumberOfReceivers() const;
      std::string getReceiverPrefix() const;
    private:
      const YAML::Node root;
  };
}