#include "types.h"
#include "user.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"

int main(int argc, char *argv[]) {
  char *username = "myuser";
  char *password = "mypassword";

  int status = login(username, password);
  if (status == 0) {
    printf(1, "Authentication successful\n");
  } else {
    printf(1, "Authentication failed\n");
  }

  exit();
}

