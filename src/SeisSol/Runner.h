#pragma once

namespace SeisSol {

  class Runner {
    public:
      Runner(char * path);
      int run() const;
    
    private:
      char * binaryPath;

  };

}