#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

long strtol(const char *str, char **endptr, int base) {
    if (base < 0 || base == 1 || base > 36) {
        *endptr = (char *)str;
        return 0;
    }

    long result = 0;
    int sign = 1;
    const char *ptr = str;

    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') {
        ptr++;
    }

    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    if (base == 0) {
        if (*ptr == '0') {
            if (*(ptr + 1) == 'x' || *(ptr + 1) == 'X') {
                base = 16;
                ptr += 2;
            } else {
                base = 8;
                ptr++;
            }
        } else {
            base = 10;
        }
    }

    const char *start = ptr;
    while (1) {
        int digit;
        if (*ptr >= '0' && *ptr <= '9') {
            digit = *ptr - '0';
        } else if (*ptr >= 'a' && *ptr <= 'z') {
            digit = *ptr - 'a' + 10;
        } else if (*ptr >= 'A' && *ptr <= 'Z') {
            digit = *ptr - 'A' + 10;
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        ptr++;
    }

    if (endptr != NULL) {
        *endptr = (char *)(ptr == start ? str : ptr);
    }

    return sign * result;
}



int parse_mode(const char *mode_str) {
    if (strcmp(mode_str, "u+s") == 0) {
        return S_ISUID;
    } else if (strcmp(mode_str, "g+s") == 0) {
        return S_ISGID;
    } else {
        char *endptr;
        int mode = (int)strtol(mode_str, &endptr, 8);
        if (*endptr != '\0') {
            printf(1, "Error: Invalid mode format\n");
            exit();
        }
        return mode;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf(1, "Usage: chmod MODE filepath\n");
	exit();
    }

    int mode = parse_mode(argv[1]);
    char *filepath = argv[2];

    struct stat st;
    if (stat(filepath, &st) < 0) {
        printf(1, "Error: Cannot stat file %s\n", filepath);
        exit();
    }

    // Check if the user is the owner of the file or the superuser
    if (st.uid != getuid() && getuid() != 0) {
        printf(1, "Error: Permission denied\n");
        exit();
    }

    if (chmod(filepath, mode) < 0) {
        printf(1, "Error: Failed to change mode of file %s\n", filepath);
        exit();
    }

    printf(1, "Mode changed successfully for file %s\n", filepath);
    exit();
}

