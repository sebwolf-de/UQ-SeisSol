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

void SeisSol::Runner::run(size_t index) const {
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
    
    std::vector<std::string> parameterFiles = {"parameters_0.par", "parameters_1.par", "parameters_2.par"};
    seissolError = execlp("srun", "srun", binaryPath.c_str(), parameterFiles[index].c_str(), NULL);

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
    //TODO: use C++20 std::format
    char buffer [5];
    sprintf (buffer, "%05i", runCount);
    const std::string output = chain + "/receiver-" + std::to_string(it->first) + "-chain-" + buffer + ".dat";
    boost::filesystem::copy_file(it->second, output);
  }
  boost::filesystem::remove_all(current);
  boost::filesystem::create_directory(current);
}
