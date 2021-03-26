#pragma once

#include <Eigen/Dense>
#include <fstream>
#include <map>

namespace IO {

class MaterialParameterWriter {
  public:
  MaterialParameterWriter(std::string templateFileContent,
                          std::vector<std::string> parameterKeys);

  void updateParameters(Eigen::VectorXd parameters) const;
  size_t numberOfParameters() const;

  private:
  const std::string templateFileContent;
  const std::vector<std::string> parameterKeys;
};

} // namespace IO
