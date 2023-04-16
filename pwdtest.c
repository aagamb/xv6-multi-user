#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"
#include "stat.h"

int main(void) {
  printf(1, "=== Starting pwd test ===\n");

  // Test 1: pwd in the current directory
  printf(1, "Test 1: pwd in the current directory\n");
  if (fork() == 0) {
    char *argv[] = {"pwd", 0};
    exec("pwd", argv);
  } else {
    wait();
  }

  // Create a test directory and subdirectories
  mkdir("testdir");
  mkdir("testdir/subdir1");
  mkdir("testdir/subdir2");

  // Test 2: Change to a subdirectory and test pwd
  printf(1, "Test 2: Change to a subdirectory and test pwd\n");
  chdir("testdir/subdir1");
  if (fork() == 0) {
    char *argv[] = {"pwd", 0};
    exec("pwd", argv);
  } else {
    wait();
  }

  // Test 3: Change to another subdirectory and test pwd
  printf(1, "Test 3: Change to another subdirectory and test pwd\n");
  chdir("../subdir2");
  if (fork() == 0) {
    char *argv[] = {"pwd", 0};
    exec("pwd", argv);
  } else {
    wait();
  }

  // Test 4: Go back to the parent directory and test pwd
  printf(1, "Test 4: Go back to the parent directory and test pwd\n");
  chdir("..");
  if (fork() == 0) {
    char *argv[] = {"pwd", 0};
    exec("pwd", argv);
  } else {
    wait();
  }

  printf(1, "=== Ending pwd test ===\n");
  exit();
}

