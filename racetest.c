#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  printf(1, "Starting concurrency test with 2 processes...\n");

  int pid = fork();

  if (pid < 0) {
    printf(1, "Fork failed!\n");
    exit();
  }

  // Both parent and child invoke the kernel increment function concurrently
  race_inc();

  if (pid > 0) {
    // Parent waits for child to terminate
    wait();
    
    int final_val = race_get();
    printf(1, "\n==========================================\n");
    printf(1, "Final Counter Value : %d\n", final_val);
    printf(1, "Expected Value      : 200000\n");
    printf(1, "==========================================\n\n");
  }

  exit();
}