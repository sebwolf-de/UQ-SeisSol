#include "Runner.h"

#include <ctime>
#include <filesystem>
#include <map>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "IO/ReceiverReader.h"
#include "spdlog/spdlog.h"

SeisSol::Runner::Runner(std::string seisSolBinaryPath, std::string mpiCommand,
                        std::string receiverPrefix)
    : binaryPath(std::move(seisSolBinaryPath)), mpiCommand(std::move(mpiCommand)),
      receiverPrefix(std::move(receiverPrefix)) {}

int SeisSol::Runner::run(size_t index) const {
  int status = 0;
  const int pid = fork();

  // pid < 0 means the child process creation failed
  // pid == 0 means we are in the child process
  // pid > 0 means we are in the parent process
  if (pid < 0) {
    spdlog::error("pid value: {}", pid);
    spdlog::error("Child process creation to run SeisSol was unsuccessful, exiting UQ-SeisSol.");
    exit(1);
  } else if (pid == 0) {
    freopen("SeisSol_stdout.txt", "a", stdout);
    freopen("SeisSol_stderr.txt", "w", stderr);

    // execl returns -1 if there was an error
    // execl does not return if the command was successful

    const std::string parameterFile = "parameters_" + std::to_string(index) + ".par";
    const int seissolError =
        execlp(mpiCommand.c_str(), mpiCommand.c_str(), "--bind-to", "none", "-n", "4",
               "./launch.sh", binaryPath.c_str(), parameterFile.c_str(), NULL);

    if (seissolError == -1) {
      exit(1);
    } else {
      exit(0);
    }
  }
  waitpid(pid, &status, 0);

  if (WEXITSTATUS(status) != 0) {
    spdlog::warn("SeisSol exited with an error exit code.");
    spdlog::warn("You can check 'SeisSol_stderr.txt' for more information.");
  }
  return status;
}

void SeisSol::Runner::prepareFilesystem(size_t runCount) {
  // Copies the old receiver data to output/chain and removes them from output/current
  const std::string chainDirectory = "output/chain";
  if (!std::filesystem::exists(chainDirectory)) {
    std::filesystem::create_directory(chainDirectory);
  }
  const std::string currentDirectory = "output/current";
  const auto receiverList = IO::getReceiversInDirectory(currentDirectory, receiverPrefix);
  // TODO: use C++20 std::format
  char buffer[6];
  sprintf(buffer, "%05lu", runCount);
  for (const auto& it : receiverList) {
    const std::string newFilename =
        chainDirectory + "/receiver-" + std::to_string(it.first) + "-chain-" + buffer + ".dat";
    spdlog::debug("cp {} {}", it.second, newFilename);
    std::filesystem::copy(it.second, newFilename);
  }
  const std::string newDirectory = chainDirectory + "/output-" + buffer;
  if (std::filesystem::exists(currentDirectory)) {
    std::filesystem::copy(currentDirectory, newDirectory, std::filesystem::copy_options::recursive);
  }

  spdlog::info("Copied latest SeisSol output to output/chain.");
  std::filesystem::remove_all(currentDirectory);
  std::filesystem::create_directory(currentDirectory);
  spdlog::info("Cleared output/current.");
}

void SeisSol::Runner::archivePreviousRun() {
  // Archives receiver output from previous UQ-SeisSol runs
  const std::string chainDirectory = "output/chain";

  if (std::filesystem::exists(chainDirectory)) {
    spdlog::info("Archiving the previous run...");

    const std::string archiveDirectory = "output_archive";
    if (!std::filesystem::exists(archiveDirectory)) {
      std::filesystem::create_directory(archiveDirectory);
    }

    const auto now = std::chrono::system_clock::now();
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    const auto timestamp = ss.str();
    const std::string newDirectory = archiveDirectory + "/run-" + timestamp;
    std::filesystem::copy(chainDirectory, newDirectory);

    std::filesystem::remove_all(chainDirectory);
    std::filesystem::create_directory(chainDirectory);

    const std::string currentDirectory = "output/current";
    std::filesystem::remove_all(currentDirectory);
    std::filesystem::create_directory(currentDirectory);

    spdlog::info("Archiving done.");
  }
}
