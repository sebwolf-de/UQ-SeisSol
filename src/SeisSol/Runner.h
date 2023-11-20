#pragma once

#include <string>

namespace SeisSol {

class Runner {
  public:
  Runner(std::string seisSolBinaryPath, std::string mpiCommand, std::string receiverPrefix);
  [[nodiscard]] int run(size_t index) const;
  void prepareFilesystem(size_t runCount);
  static void archivePreviousRun();

  private:
  const std::string binaryPath;
  const std::string mpiCommand;
  const std::string receiverPrefix;
};

} // namespace SeisSol
