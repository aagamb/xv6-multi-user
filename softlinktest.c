#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"
#include "stat.h"
#include "softlink.h"


void print_test_result(int test_number, int result) {
    if (result) {
        printf(1, "Test %d: Passed\n", test_number);
    } else {
        printf(1, "Test %d: Failed\n", test_number);
    }
}

int
strncmp(const char *p, const char *q, uint n)
{
  while(n > 0 && *p && *p == *q)
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (uchar)*p - (uchar)*q;
}

int main(void) {
    printf(1, "=== Starting soft link test ===\n");

    int fd;
    char buf[128];
    char buf2[128];
    struct stat st;

    // Test 1: Create a soft link to an existing file and check if the contents are the same.
    printf(1, "Test 1: Create a soft link to an existing file and check if the contents are the same\n");
    fd = open("file1.txt", O_WRONLY | O_CREATE);
    write(fd, "Hello, World!", 13);
    close(fd);
    create_symlink("file1.txt", "symlink1");
    read_symlink("symlink1", buf, sizeof(buf));
    fd = open(buf, O_RDONLY);
    read(fd, buf2, sizeof(buf2));
    close(fd);
    print_test_result(1, strncmp(buf2, "Hello, World!", 13) == 0);

    // Test 2: Create a soft link to a non-existent file and check if the error is handled.
    printf(1, "Test 2: Create a soft link to a non-existent file and check if the error is handled\n");
    create_symlink("non_existent_file.txt", "symlink2");
    int result = read_symlink("symlink2", buf, sizeof(buf));
    print_test_result(2, result == -1);

    // Test 3: Create a soft link to another soft link and check if the contents are the same.
    printf(1, "Test 3: Create a soft link to another soft link and check if the contents are the same\n");
    create_symlink("symlink1", "symlink3");
    read_symlink("symlink3", buf, sizeof(buf));
    fd = open(buf, O_RDONLY);
    read(fd, buf2, sizeof(buf2));
    close(fd);
    print_test_result(3, strncmp(buf2, "Hello, World!", 13) == 0);

 
    printf(1, "=== Ending soft link test ===\n");
    exit();
}
