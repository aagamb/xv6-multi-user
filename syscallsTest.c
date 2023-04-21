#include "types.h"
#include "user.h"
#include "fcntl.h"

int shared_status;

int system(const char *command) {
    int pid = fork();
    if (pid < 0) {
        printf(2, "system: fork failed\n");
        return -1;
    }

    if (pid == 0) { // Child process
        char *argv[] = {command, 0};
        if (exec(command, argv) < 0) {
            printf(2, "system: exec failed\n");
            shared_status = -1;
        }
        shared_status = 0;
        exit(); // Terminate child process after exec fails or succeeds
    } else { // Parent process
        int child_pid = wait();
        if (child_pid < 0) {
            printf(2, "system: wait failed\n");
            return -1;
        }
    }

    return shared_status;
}

void test_syscalls() {
  printf(1, "----- Testing System Calls -----\n");

  if (getuid()<0){
    printf(1, "getuid() failed\n");
  }else{
    printf(1, "getuid() succeeded\n");
  }

  if (geteuid()<0){
    printf(1, "geteuid() failed\n");
  }else{
    printf(1, "geteuid() succeeded\n");
  }
  

  if (setuid(0)<0){
    printf(1, "setuid() failed\n");
  }else if(getuid()==0){
    printf(1, "setuid() succeeded\n");
  }else{
    printf(1, "setuid() failed\n");
  }

  if (seteuid(0)<0){
    printf(1, "seteuid() failed\n");
  }else if (geteuid()==0){
    printf(1, "seteuid() succeeded\n");
  }else{
    printf(1, "seteuid() failed\n");
  }

  if (setegid(0)<0){
    printf(1, "setegid() failed\n");
  }else{
    printf(1, "setegid() succeeded\n");
  }

  if(setreuid(0,0)<0){
    printf(1, "setreuid() failed\n");
  }else if(getuid()==0 && geteuid()==0){
    printf(1, "setreuid() succeeded\n");
  }else{
    printf(1, "setreuid() failed\n");
  }

  if(setregid(0,0)<0){
    printf(1, "setregid() failed\n");
  }else{
    printf(1, "setregid() succeeded\n");
  }

//   system("whoami");

  int result = setuid(1000);
  printf(1, "setuid(1000) returned: %d\n", result);

  int new_uid = getuid();
  printf(1, "New UID: %d\n", new_uid);

  // Add tests for other system calls in a similar manner

  printf(1, "----- System Calls Test Complete -----\n");

}

int main() {
  test_syscalls();
  char *argv[] = {"ls", 0};

  exit();
}
