#pragma once

#include <string>

namespace SeisSol {

class Runner {
  public:
  Runner(std::string seisSolBinaryPath, std::string parametersFilePath, size_t numberOfProcesses);
  void run();

  private:
  const std::string binaryPath;
  const std::string parametersPath;
  const size_t numberOfProcesses;
  size_t runCount = 0;
};

} // namespace SeisSol
