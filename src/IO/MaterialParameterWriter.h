#pragma once

#include <Eigen/Dense>
#include <fstream>
#include <map>

namespace IO {

class MaterialParameterWriter {
  public:
  MaterialParameterWriter(const std::string& templateFileContent,
                          const std::string& outputFilename,
                          const std::vector<std::string>& parameterKeys);
  void updateParameters(Eigen::VectorXd parameters) const;
  size_t numberOfParameters() const;

  private:
  const std::string templateFileContent;
  const std::string outputFilename;
  const std::vector<std::string> parameterKeys;
};

} // namespace IO
