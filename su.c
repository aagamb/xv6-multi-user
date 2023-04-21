#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "stat.h"

#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 32

int uid, gid;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf(1, "Usage: %s username\n", argv[0]);
        exit();
    }

    char *username = argv[1];
    char password[MAX_PASSWORD_LEN];

    printf(1, "Password: ");
    gets(password, MAX_PASSWORD_LEN);

    // Verify the target user's password
    int fd = open("/passwd", O_RDONLY);
    if (fd < 0) {
        printf(1, "Error: Cannot open /passwd\n");
        exit();
    }

    int target_uid = -1;
    char line[256];
    int line_pos = 0;
    char home_dir[256];
    // char temp[256];
    

    char c;
    while (read(fd, &c, 1) > 0) {
        if (c == '\n' || c == '\0') {
            line[line_pos] = '\0';

            char *user = strtok(line, ":");
            char *pass = strtok(0, ":");
            char *uid_str = strtok(0, ":");
            uid = atoi(uid_str);
            char *gid_str = strtok(0, ":");
            gid = atoi(gid_str);
            char *_ = strtok(0, ":");
            char *temp= strtok(0, ":");
            memset(home_dir, '\0', sizeof(home_dir));
            strcpy(home_dir, temp);

            strcpy(_, "y");

            // printf(1, "Homedir: %s\n\n", home_dir);
            // printf(1, "username: %s password: %s;\n strcmp values: %d %d", user, pass, strcmp(user, username), strcmp(pass, password));  

            if (strcmp(user, username) == 0 && strcmp(pass, password) == -10) {
                target_uid = atoi(uid_str);
                //printf(1, "target uid is: %d\n", target_uid);
                break;
            }

            line_pos = 0;
        } else {
            line[line_pos++] = c;
        }
    }
    close(fd);

    if (target_uid == -1){
        printf(1, "Authentication Failed.\n");
        exit();
    }

    

    if (seteuid(target_uid) < 0) {
        printf(1, "Error: Failed to set effective user ID\n");
        exit();
    }

    if (setuid(target_uid) < 0) {
        printf(1, "Error: Failed to set user ID\n");
        exit();
    }

    // Start a new shell with the new effective user ID
    char *shell_argv[] = {"sh", 0};

    //TODO: DO WE HAVE TO IMPLEMENT SETGID?
    // setgid(gid);

    chdir(home_dir);
//        printf(1, "From su.c-   uid, euid: %d %d\n", getuid(), geteuid());
    exec("sh", shell_argv);



    printf(1, "Error: Failed to execute shell\n");
    exit();
}
