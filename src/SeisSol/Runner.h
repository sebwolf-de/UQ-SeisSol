#pragma once

#include <string>

namespace SeisSol {

  class Runner {
    public:
      Runner(std::string seisSolBinaryPath, std::string parametersFilePath);
      void run();
    
    private:
      const std::string binaryPath;
      const std::string parametersPath;
      size_t runCount = 0;
  };

}
