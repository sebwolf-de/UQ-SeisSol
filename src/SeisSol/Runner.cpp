#include "Runner.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

SeisSol::Runner::Runner(std::string seisSolBinaryPath, std::string parametersFilePath)
    : binaryPath(seisSolBinaryPath), parametersPath(parametersFilePath) {}

void SeisSol::Runner::run() {
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
    freopen("/dev/null", "a", stdout);
    freopen("SeisSol_stderr.txt", "w", stderr);

    // Cleans the output directory of past receiver outputs
    boost::filesystem::remove_all("output");
    boost::filesystem::create_directory("output");

    // execl returns -1 if there was an error
    // execl does not return if the command was successful
    seissolError =
        execlp("mpiexec", "mpiexec", "-n", "28", binaryPath.c_str(), parametersPath.c_str(), NULL);

    if (seissolError == -1)
      exit(1);
  } else {
    waitpid(pid, &status, 0);

    if (WEXITSTATUS(status) != 0) {
      std::cout << "SeisSol exited with an error exit code, exiting UQ-SeisSol." << std::endl;
      std::cout << "You can check 'SeisSol_stderr.txt' for more information." << std::endl;
      exit(1);
    }

    std::cout << "Executed SeisSol " << ++runCount << " times." << std::endl;
  }
}
