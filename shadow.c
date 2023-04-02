#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define BUFSIZE 512

int
main(int argc, char *argv[])
{
int fd;
  if ((fd = open("shadow.txt", O_RDONLY)) < 0) {
    printf(2, "cannot open shadow.txt\n");
    exit();
  }

  fd = open("shadow.txt", O_RDONLY);
  int new_fd = open("/etc/shadow.txt", O_CREATE);
  char buf[512];
  int n = read(fd, buf, sizeof(buf));
  write(new_fd, buf, n);	
  close(fd);
close(new_fd);
unlink("shadow.txt");
  

if (chdir("/etc") != 0) {
  printf(1, "Failed to change directory to /etc\n");
  exit();
}

char* execArg[] = { "sh", 0 };
exec(argv[0], execArg);
  exit();
}

