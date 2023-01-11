#include "Runner.h"

#include <map>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

#include "IO/ReceiverReader.h"
#include "spdlog/spdlog.h"
#include <boost/filesystem.hpp>

SeisSol::Runner::Runner(std::string seisSolBinaryPath) : binaryPath(std::move(seisSolBinaryPath)) {}

int SeisSol::Runner::run(size_t index) const {
  int status;
  int seissolError;
  int pid = fork();

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

    std::string parameterFile = "parameters_" + std::to_string(index) + ".par";
    seissolError = execlp("mpiexec", "mpiexec", binaryPath.c_str(), parameterFile.c_str(), NULL);

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
  std::string chain = "output/chain";
  if (!boost::filesystem::exists(chain)) {
    boost::filesystem::create_directory(chain);
  }
  std::string current = "output/current";
  const auto receiverList = IO::getReceiversInDirectory(current, "output-receiver");
  for (const auto& it : receiverList) {
    // TODO: use C++20 std::format
    char buffer[6];
    sprintf(buffer, "%05lu", runCount);
    const std::string output =
        chain + "/receiver-" + std::to_string(it.first) + "-chain-" + buffer + ".dat";
    boost::filesystem::copy_file(it.second, output);
  }
  boost::filesystem::remove_all(current);
  boost::filesystem::create_directory(current);
}

void SeisSol::Runner::archivePreviousRun() {
  // Archives receiver output from previous UQ-SeisSol runs
  std::string chainDirectory = "output/chain";

  if (boost::filesystem::exists(chainDirectory)) {
    spdlog::info("Archiving the previous run...");

    std::string archiveDirectory = "output_archive";
    if (!boost::filesystem::exists(archiveDirectory)) {
      boost::filesystem::create_directory(archiveDirectory);
    }

    size_t numOfArchivedRuns = 0;

    for ([[maybe_unused]] boost::filesystem::directory_entry& e :
         boost::filesystem::directory_iterator(archiveDirectory)) {
      numOfArchivedRuns += 1;
    }

    boost::filesystem::create_directory(archiveDirectory + "/run-" +
                                        std::to_string(numOfArchivedRuns + 1));

    for (boost::filesystem::directory_entry& e :
         boost::filesystem::directory_iterator(chainDirectory)) {
      boost::filesystem::copy_file(e.path(), archiveDirectory + "/run-" +
                                                 std::to_string(numOfArchivedRuns + 1) + "/" +
                                                 e.path().filename().string());
    }

    boost::filesystem::remove_all(chainDirectory);
    boost::filesystem::create_directory(chainDirectory);

    spdlog::info("Archiving done.");
  }
}
