#include "Runner.h"

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

SeisSol::Runner::Runner(char path[]) : binaryPath(path) {};

int SeisSol::Runner::run() const {
  int pid, status;

  if (pid = fork()) {
    waitpid(pid, &status, 0);
  } else {
    execl(binaryPath, binaryPath, NULL);
  }

  std::cout << "Executed SeisSol." << std::endl;

  return 0;
}
