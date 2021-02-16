#pragma once

#include <map>
#include <fstream>
#include <Eigen/Dense>

namespace IO {

  class MaterialParameterWriter {
    public:
      MaterialParameterWriter(
        std::string templateFileContent,
        size_t numberOfParameters,
        std::map<int, std::string> parameterKeys
      );
      void updateParameters(Eigen::VectorXd parameters);
      void save();
      const size_t numberOfParameters;

    private:
      const std::string templateFileContent;
      const std::map<int, std::string> parameterKeys;
      std::string unsavedContent;
  };

}
