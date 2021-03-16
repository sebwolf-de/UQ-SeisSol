#pragma once

#include <string>

namespace SeisSol {

class Runner {
  public:
  Runner(std::string seisSolBinaryPath, std::string parametersFilePath,
         std::string numberOfProcesses);
  void run();

  private:
  const std::string binaryPath;
  const std::string parametersPath;
  const std::string numberOfProcesses;
  size_t runCount = 0;
};

} // namespace SeisSol
