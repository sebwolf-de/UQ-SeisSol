#pragma once

#include <Eigen/Dense>
#include <fstream>
#include <map>

#include <vector>

namespace IO {

class ChainParameterWriter {
  public:
  ChainParameterWriter(std::string templateFileContent, std::string outputFilename,
                       std::vector<std::string> parameterKeys);
  void updateParameters(Eigen::VectorXd parameters) const;
  void updateParameters(std::vector<Eigen::VectorXd> parameters) const;
  [[nodiscard]] size_t numberOfParameters() const;

  private:
  const std::string templateFileContent;
  const std::string outputFilename;
  const std::vector<std::string> parameterKeys;
};

} // namespace IO