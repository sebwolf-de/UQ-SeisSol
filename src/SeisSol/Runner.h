#pragma once

#include <string>

namespace SeisSol {

  class Runner {
    public:
      Runner(std::string seisSolBinaryPath, std::string parametersFilePath);
      int run() const;
    
    private:
      std::string binaryPath;
      std::string parametersPath;

  };

}
