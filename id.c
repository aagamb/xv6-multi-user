#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void print_uint(int x) {
    if (x == 0) {
        printf(1, "0");
        return;
    }

    char buf[16];
    int i = 0;
    while (x > 0) {
        buf[i++] = (x % 10) + '0';
        x /= 10;
    }

    while (--i >= 0) {
        printf(1, "%c", buf[i]);
    }
}

void parse_file(int fd, int target, char *buf, int buflen) {
    int n, i = 0, field_start;
    char c;
    int field_counter = 0;
    int found = 0;

    buf[0] = '\0';
    while ((n = read(fd, &c, 1)) > 0) {
        if (c == ':' || c == '\n') {
            if (field_counter == 2 && found) {
                buf[i - field_start] = '\0';
                break;
            }

            if (field_counter == 0) {
                field_start = i;
                field_counter++;
            } else if (field_counter == 1) {
                field_counter++;
            } else if (c == '\n') {
                field_counter = 0;
            }
        } else {
            buf[i++] = c;
            if (field_counter == 2 && atoi(buf) == target) {
                found = 1;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int uid, gid;
    char username[32], groupname[32];
    int passwd_fd, group_fd;

    uid = getuid();
    gid = getuid();

    passwd_fd = open("/passwd", O_RDONLY);
    if (passwd_fd < 0) {
        printf(1, "Failed to open /passwd\n");
        exit();
    }
    parse_file(passwd_fd, uid, username, sizeof(username));
    close(passwd_fd);

    group_fd = open("/group", O_RDONLY);
    if (group_fd < 0) {
        printf(1, "Failed to open /group\n");
        exit();
    }
    parse_file(group_fd, gid, groupname, sizeof(groupname));
    close(group_fd);

    printf(1, "(");
    print_uint(uid);
    printf(1, ")%s gid(", username);
    print_uint(gid);
    printf(1, ")%s groups=", groupname);
    print_uint(gid);
    printf(1, "(%s)\n", groupname);

    exit();
}

