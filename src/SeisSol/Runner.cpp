#include "Runner.h"

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

SeisSol::Runner::Runner(std::string path) : binaryPath(path) {};

int SeisSol::Runner::run() const {
  int pid, status;

  if (pid = fork()) {
    waitpid(pid, &status, 0);
  } else {
    execl(binaryPath.c_str(), binaryPath.c_str(), NULL);
  }

  std::cout << "Executed SeisSol." << std::endl;

  return 0;
}
