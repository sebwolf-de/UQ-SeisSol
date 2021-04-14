#include "Runner.h"

#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <map>

#include <boost/filesystem.hpp>
#include "IO/ReceiverReader.h"

SeisSol::Runner::Runner(std::string seisSolBinaryPath, std::string parametersFilePath)
    : binaryPath(seisSolBinaryPath), parametersPath(parametersFilePath) {}

void SeisSol::Runner::run() const {
  int status;
  int seissolError;
  int pid = fork();

  // pid < 0 means the child process creation failed
  // pid == 0 means we are in the child process
  // pid > 0 means we are in the parent process
  if (pid < 0) {
    std::cout << "pid value: " << pid << std::endl;
    std::cout << "Child process creation to run SeisSol was unsuccessful, exiting UQ-SeisSol. "
              << std::endl;
    exit(1);
  } else if (pid == 0) {
    freopen("SeisSol_stdout.txt", "a", stdout);
    freopen("SeisSol_stderr.txt", "w", stderr);

    // execl returns -1 if there was an error
    // execl does not return if the command was successful
    seissolError = execlp("srun", "srun", binaryPath.c_str(), parametersPath.c_str(), NULL);

    if (seissolError == -1)
      exit(1);
  } else {
    waitpid(pid, &status, 0);

    if (WEXITSTATUS(status) != 0) {
      std::cout << "SeisSol exited with an error exit code, exiting UQ-SeisSol." << std::endl;
      std::cout << "You can check 'SeisSol_stderr.txt' for more information." << std::endl;
      exit(1);
    }
  }
}

void SeisSol::Runner::prepareFilesystem(size_t runCount) const {
  // Copies the old receiver data to output/chain and removes them from output/current
  std::string chain = "output/chain";
  if (!boost::filesystem::exists(chain)) {
    boost::filesystem::create_directory(chain);
  }
  std::string current = "output/current";
  const auto receiverList = IO::getReceiversInDirectory(current, "output-receiver");
  for (auto it = receiverList.begin(); it != receiverList.end(); it++) {
    boost::filesystem::copy_file(it->second, chain + "/receiver-" + std::to_string(it->first) + "-chain-" + std::to_string(runCount) + ".dat");
  }
  boost::filesystem::remove_all(current);
  boost::filesystem::create_directory(current);
}
