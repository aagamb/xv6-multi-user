#include "types.h"
#include "stat.h"
#include "user.h"
//#include "fs.h"
#include "fcntl.h"
#include "myuser.h"


struct user* userArray[100];
int numUsers =0;
int uidNumber = 0;
int gidNumber =0;

int open_with_root_permissions(const char *path, int flags) {
    // Save the current EUID and set it to 0 (superuser)
    int original_euid = geteuid();
    seteuid(0);

    // Perform the open operation with elevated privileges
    int fd = open(path, flags);

    // Restore the original EUID
    seteuid(original_euid);

    

    return fd;
}

// char *strtok(char *str, const char *delim) {
//   static char *last;
//   if (str != NULL) {
//     last = str;
//   }
//   if (last == NULL || *last == '\0') {
//     return NULL;
//   }
//   char *token = last;
//   while (*last != '\0') {
//     if (strchr(delim, *last) != NULL) {
//       *last++ = '\0';
//       break;
//     }
//     last++;
//   }
//   return token;
// }

struct user* userFromPasswdLine(char* s)
{
    struct user* currUser = (struct user*)malloc(sizeof(struct user));
    if (currUser == NULL) {
        return NULL;
    }

    char tmp[7][100];

    int k = 0;
    
    // get the first token
    char* token = strtok(s, ":");

    // walk through other tokens
    while (token) {
        strcpy(tmp[k++],token);
        token = strtok(NULL, ":");
    }

   strcpy(currUser->username,tmp[0]);
   strcpy(currUser->password,tmp[1]);
   currUser->uid=atoi(tmp[2]);
   currUser->gid=atoi(tmp[3]);
   strcpy(currUser->personName,tmp[4]);
   strcpy(currUser->homedir,tmp[5]);


    return currUser;
}

void process_buffer(char *buffer, int length) {
    int start = 0;

    for (int i = 0; i < length; ++i) {
        if (buffer[i] == '\n' || i == length - 1) {
            if (i == length - 1 && buffer[i] != '\n') {
                // Include the last character if it's not a newline
                i++;
            }
            buffer[i] = '\0'; // Replace the newline with a null character
            // printf(1, "Line: %s\n", &buffer[start]);
            struct user* currUser=userFromPasswdLine(&buffer[start]);
            userArray[numUsers++] = currUser;
            uidNumber++;
            gidNumber++;
            start = i + 1;
        }
    }
}

void createUserArray(){
    int fd = open_with_root_permissions("/passwd", O_RDONLY);
    seteuid(0);
    if (fd < 0) {
        printf(2, "Error: cannot open file %s\n", "passwd");
        exit();
    }


    char buffer[1024];
    int bytes_read = 0;

    while ((bytes_read = read(fd, buffer, 1024 - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Ensure the buffer is null-terminated
        process_buffer(buffer, bytes_read);
    }

    close(fd);

    if (bytes_read < 0) {
        printf(2, "Error: cannot read file %s\n", "passwd");
        exit();
    }
}

int main(int argc, char const *argv[])
{   
    int euid = geteuid();
    seteuid(0);
    createUserArray();
    seteuid(euid);
    

    for(int i=0; i<numUsers; i++){
        if(getuid() == userArray[i]->uid){
            printf(1, "%s\n", userArray[i]->username);
            exit();
        }
    }

    exit();
}
