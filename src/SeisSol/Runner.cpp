#include "Runner.h"

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

SeisSol::Runner::Runner(char path[]) : binaryPath(path) {};

int SeisSol::Runner::run() const {
  int status;
  int seissolError;
  int pid = fork();

  // pid < 0 means the child process creation failed
  // pid == 0 means we are in the child process
  // pid > 0 means we are in the parent process
  if (pid < 0) {
    std::cout << "pid value: " << pid << std::endl;
    std::cout << "Child process creation to run SeisSol was unsuccessful, exiting UQ-SeisSol. " << std::endl;
    exit(1);
  } else if (pid == 0) {
    // execl returns -1 if there was an error
    // execl does not return if the command was successful
    seissolError = execl(binaryPath, binaryPath, NULL);

    if (seissolError == -1) exit(1);
  } else {
    waitpid(pid, &status, 0);

    if (WEXITSTATUS(status) != 0) {
      std::cout << "SeisSol exited with an error exit code, exiting UQ-SeisSol." << std::endl;
      exit(1);
    }

    std::cout << "Executed SeisSol." << std::endl;
  }

  return 0;
}
