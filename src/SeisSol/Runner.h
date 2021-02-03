#pragma once

#include <string>

namespace SeisSol {

  class Runner {
    public:
      Runner(std::string path);
      int run() const;
    
    private:
      std::string binaryPath;

  };

}
