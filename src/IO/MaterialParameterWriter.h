#pragma once

#include <map>
#include <fstream>
#include <Eigen/Dense>

namespace IO {

  class MaterialParameterWriter {
    public:
      MaterialParameterWriter(
        const std::string & templateFileContent,
        const std::vector<std::string> & parameterKeys
      );
      void updateParameters(Eigen::VectorXd parameters) const;
      const size_t numberOfParameters() const;

    private:
      const std::string templateFileContent;
      const std::vector<std::string> parameterKeys;
  };

}
