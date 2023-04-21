#include "types.h"
#include "stat.h"
#include "user.h"
//#include "fs.h"
#include "fcntl.h"
#include "myuser.h"

struct user* userArray[100];
int numUsers =0;
int uidNumber=1000;
int gidNumber = 0;

struct user* createUser(char username[]){

    char fname[100];
    char password[100];
    printf(1, "Enter Full Name: ");
    gets(fname, 100);
    fname[strlen(fname)-1] = '\0';
    printf(1, "Password: ");
    gets(password, 100);
    password[strlen(password)-1] = '\0';

    char dir[100];
    strcpy(dir, "/home/");
    strcat(dir, username);
    // mkdir(dir);

    struct user* u = (struct user*) malloc(sizeof(struct user));
    strcpy(u->username, username);
    strcpy(u->password, password);
    u->uid = uidNumber++;
    u->gid = gidNumber++;
    strcpy(u->personName, fname);
    strcpy(u->homedir, dir);
    strcpy(u->shell, "/sh");

    return u;
}

void addUserToPasswd(struct user* u){

    char res[100] ="";
    char uidStr[100];
    char gidStr[100];

    itoa(u->gid, gidStr);
    itoa(u->uid, uidStr);

    // char toAppend [7][100] = {u->username, u->password, itoa(u->uid), itoa(u->gid,) u->personName, u->homedir, itoa(u->mode)};

    // strcat(res, "\n");
    strcat(res, u->username);
    strcat(res, ":");   

    strcat(res, u->password);
    strcat(res, ":");

    strcat(res, uidStr);
    strcat(res, ":");

    strcat(res, gidStr);
    strcat(res, ":");

    strcat(res, u->username);
    strcat(res, ":");

    strcat(res, u->homedir);
    strcat(res, ":");

    strcat(res, u->shell);
    strcat(res, "\n");

    // printf(1, "line to be added to passwd is: %s\n", res);
    
    const char *file_name = "/passwd";
    const char *string_to_append = res;

    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        printf(2, "Error: cannot open file %s\n", file_name);
        exit();
    }

    char buffer[1024];
    int bytes_read = 0;
    int total_bytes_read = 0;

    while ((bytes_read = read(fd, buffer + total_bytes_read, 1024 - total_bytes_read - 1)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read == 1024 - 1) {
            printf(2, "Error: file too large to read into buffer\n");
            close(fd);
            exit();
        }
    }

    close(fd);

    if (bytes_read < 0) {
        printf(2, "Error: cannot read file %s\n", file_name);
        exit();
    }

    int append_len = strlen(string_to_append);
    memmove(buffer + total_bytes_read, string_to_append, append_len);
    total_bytes_read += append_len;

    fd = open(file_name, O_WRONLY | O_CREATE);
    if (fd < 0) {
        printf(2, "Error: cannot open file %s\n", file_name);
        exit();
    }

    int bytes_written = write(fd, buffer, total_bytes_read);
    if (bytes_written != total_bytes_read) {
        printf(2, "Error: cannot write to file %s\n", file_name);
    }

    close(fd);
}

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
    int fd = open("/passwd", O_RDONLY);
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

//TODO fix isUsernameTaken with the help of addUserstoArray check if that works
int isUsernameTaken(char* username){
    for (int k=0; k<numUsers; k++){
        // printf(1, "username[i] is: %s\n", userAr`ray[k]->username);
        // printf(1, "strcmp value %d\n", strcmp(username, userArray[k]->username));
        if (!strcmp(username, userArray[k]->username)){
            printf(1, "Username already taken\n");
            // printf(1, "username[i] is: %s\n", userArray[k]->username);
            // printf(1, "strcmp value %d\n", strcmp(username, userArray[k]->username));
            return 1;
        }
    }
    return 0;
}

void printUserArray(){
    for (int k =0; k<numUsers; k++){
        printf(1, "User %d username: %s\n", k, userArray[k]->username);
    }
    // exit();
}

void addGidAndUsername(int gid, const char *username) {
  int fd, n;
  int gid_found = 0;
  char buf[512];

  // Open the "/etc/group" file for reading
  if ((fd = open("/group", O_RDONLY)) < 0) {
    printf(2, "Error: Failed to open /group\n");
    return;
  }

  // Read the file into the buffer
  n = read(fd, buf, sizeof(buf) - 64); // Reserve space for the new line
  buf[n] = '\0';
  close(fd);

  // Parse the file line by line
  char *line = strtok(buf, "\n");
  while (line != 0) {
    int line_gid = atoi(strtok(line, ":"));
    if (line_gid == gid) {
      gid_found = 1;
      break;
    }
    line = strtok(0, "\n");
  }

  // If the gid is not found, add a new line with the gid and username
  if (!gid_found) {
    // Write gid to the buffer
    // n += itoa(gid, buf + n);
    // char* temp = itoa(gid);
    strcpy(buf, gid);

    // Write ':' to the buffer
    buf[n++] = ':';

    // Write username to the buffer
    while (*username) {
      buf[n++] = *username++;
    }

    // Write newline and null-terminator to the buffer
    buf[n++] = '\n';
    buf[n] = '\0';

    // Open the "/etc/group" file for writing
    if ((fd = open("/group", O_WRONLY)) < 0) {
      printf(2, "Error: Failed to open /group for writing\n");
      return;
    }

    if (write(fd, buf, n) != n) {
      printf(2, "Error: Failed to write new entry to /group\n");
    }
    close(fd);
  }
}


int main(int argc, char* argv[]){

    if (argc<2){
        printf(1, "use it correctly\n");
        exit();
    }

    createUserArray();
    // printUserArray();
    if(isUsernameTaken(argv[1])){exit();}
    struct user* u = createUser(argv[1]);

    printf(1, "%s\n", u->homedir);
  //  mkdir(u->homedir);
   if( mkdir2(u->homedir, u->uid)<0){
	printf(1, "mkdir2 failed\n");
	}
	addUserToPasswd(u);
    
    // printUserArray();

	exit();
}
