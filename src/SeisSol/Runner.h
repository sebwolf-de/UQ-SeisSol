#pragma once

#include <string>

namespace SeisSol {

class Runner {
  public:
  Runner(std::string seisSolBinaryPath, std::string parametersFilePath);
  void run(size_t index) const;
  void prepareFilesystem(size_t runCount) const;
  void archivePreviousRun() const;

  private:
  const std::string binaryPath;
  const std::string parametersPath;
};

} // namespace SeisSol
