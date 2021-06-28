#pragma once

#include <string>

namespace SeisSol {

class Runner {
  public:
  Runner(std::string seisSolBinaryPath);
  void run(size_t index) const;
  void prepareFilesystem(size_t runCount) const;
  void archivePreviousRun() const;

  private:
  const std::string binaryPath;
};

} // namespace SeisSol
