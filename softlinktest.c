#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  int fd;
  int nbytes;
  char buf[512];
  char symlink_buf[512];

  // Case 1: Create a new file and write some data to it.
  fd = open("original.txt", O_CREATE | O_RDWR);
  if (fd < 0) {
    printf(1, "Failed to create original.txt\n");
    exit();
  }
  strcpy(buf, "This is a test file.\n");
  nbytes = write(fd, buf, strlen(buf));
  close(fd);

  // Case 2: Create a symlink pointing to the original file.
  if (symlink("original.txt", "symlink.txt") < 0) {
    printf(1, "Failed to create symlink.txt\n");
    exit();
  }

  // Case 3: Read the symlink and check if it points to the original file.
  memset(symlink_buf, 0, sizeof(symlink_buf));
  nbytes = readlink("symlink.txt", symlink_buf, sizeof(symlink_buf));
  if (nbytes < 0 || strcmp(symlink_buf, "original.txt") != 0) {
    printf(1, "Failed to read symlink.txt or incorrect target\n");
    exit();
  } else {
    printf(1, "symlink.txt points to %s\n", symlink_buf);
  }

  // Case 4: Test reading a nonexistent symlink.
  nbytes = readlink("nonexistent_symlink.txt", symlink_buf, sizeof(symlink_buf));
  if (nbytes >= 0) {
    printf(1, "Unexpected success when reading nonexistent symlink\n");
    exit();
  } else {
    printf(1, "Expected output when reading nonexistent symlink\n");
  }

  // Case 5: Test creating a symlink that already exists.
  if (symlink("original.txt", "symlink.txt") >= 0) {
    printf(1, "Unexpected success when creating an existing symlink\n");
    exit();
  } else {
    printf(1, "Expected output when creating an existing symlink\n");
  }

  printf(1, "All test cases passed.\n");
  exit();
}

