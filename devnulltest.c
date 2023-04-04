#include "types.h"
#include "user.h"
#include "fcntl.h"

#define TEST_SIZE 1024
#define DEV_NULL "/dev/null"

void test_read() {
    int fd = open(DEV_NULL, O_RDONLY);
    if (fd < 0) {
        printf(1, "test_read: failed to open %s\n", DEV_NULL);
        return;
    }

    char buf[TEST_SIZE];
    int n = read(fd, buf, TEST_SIZE);

    if (n != 0) {
        printf(1, "test_read: expected to read 0 bytes, but read %d bytes\n", n);
        close(fd);
        return;
    }

    printf(1, "test_read: passed\n");
    close(fd);
}

void test_write() {
    int fd = open(DEV_NULL, O_WRONLY);
    if (fd < 0) {
        printf(1, "test_write: failed to open %s\n", DEV_NULL);
        return;
    }

    char buf[TEST_SIZE];
    memset(buf, 0xFF, TEST_SIZE);
    int n = write(fd, buf, TEST_SIZE);

    if (n != TEST_SIZE) {
        printf(1, "test_write: expected to write %d bytes, but wrote %d bytes\n", TEST_SIZE, n);
        close(fd);
        return;
    }

    printf(1, "test_write: passed\n");
    close(fd);
}

int main() {
    test_read();
    test_write();
    exit();
}

