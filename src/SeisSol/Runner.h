#pragma once

#include <string>

namespace SeisSol {

class Runner {
  public:
  Runner(std::string seisSolBinaryPath);
  int run(size_t index) const;
  static void prepareFilesystem(size_t runCount);
  static void archivePreviousRun();

  private:
  const std::string binaryPath;
};

} // namespace SeisSol
