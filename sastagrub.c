#include "types.h"
#include "stat.h"
#include "fcntl.h"
//#include "user.h"
#include "myuser.h"

struct user* userArray[100];
int uidNumber= 0;
int gidNumber = 0;
int numUsers = 0;
int arrIndex;
char* argv[] = {"sh", 0};

char* strcat(char* dest, const char* src) {
    char* p = dest;
    while (*p != '\0') {
        p++;
    }
    while (*src != '\0') {
        *p = *src;
        p++;
        src++;
    }
    *p = '\0'; 
    return dest;
}

void strncpy(char *dest, const char *src, int n) {
    for (int i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (int i = strlen(src); i < n; i++) {
        dest[i] = '\0';
    }
}

char *strtok(char *str, const char *delim) {
  static char *last;
  if (str != NULL) {
    last = str;
  }
  if (last == NULL || *last == '\0') {
    return NULL;
  }
  char *token = last;
  while (*last != '\0') {
    if (strchr(delim, *last) != NULL) {
      *last++ = '\0';
      break;
    }
    last++;
  }
  return token;
}

struct user* userFromPasswd(char* s)
{
    struct user* currUser = (struct user*)malloc(sizeof(struct user));
    if (currUser == NULL) {
        return NULL;
    }

    char tmp[6][100];
    char buf[100];

    int loc = 0;
    for (int i = 0; i < 6; i++) {
        int curr = 0;
        while (s[curr+loc] != ':' && s[curr+loc] != '\0') {
            buf[curr] = s[curr+loc];
            curr++;
        }
        buf[curr] = '\0';
        strcpy(tmp[i], buf);
        loc += curr + 1;
    }

   strcpy(currUser->username,tmp[0]);
   strcpy(currUser->password,tmp[1]);
   currUser->uid=atoi(tmp[2]);
   currUser->gid=atoi(tmp[3]);
   strcpy(currUser->personName,tmp[4]);
   strcpy(currUser->homedir,tmp[5]);


    return currUser;
}

void putUsersInuserArray()
{

   int fd=open("passwd",O_RDONLY);
   char fileContent[1000];
   int i =0;


   read(fd,fileContent,1000);
   char* token = strtok(fileContent, "\n");
   while( token != NULL) {
		struct user* currUser=userFromPasswd(token);
		userArray[i++] = currUser;
		// printf(1, "username is: %s\n", currUser->username);

      token = strtok(NULL, "\n");
   }

   close(fd);
   return;
}

struct user* authenticateUser(int* arrIndex) {
    char username[100];
    char password[100];
    printf(1, "Username: ");
    gets(username, 100);
    username[strlen(username)] = '\0';
    printf(1, "Password: ");
    gets(password, 100);
    password[strlen(password)] = '\0';

    int i = 0;
    struct user* u = userArray[i];
    while(u) {
		// printf(1, "u->username: %s; u->password: %s; strcmp: %d;\n", u->username, u->password, strcmp(u->username, username) );
        if(strcmp(u->username, username) == -10 && strcmp(u->password, password)==-10) {
			printf(1, "Login Success\n\n");
            *arrIndex = i;
            return userArray[i];
        }
        u = userArray[++i];
    }
    return NULL;
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

    if(strcmp(currUser->username, "root")){
        strcpy(currUser->homedir,tmp[5]);
    }else{
        strcpy(currUser->homedir, "/");
    }
   

    if(strcmp(currUser->username, "root")){
        mkdir(currUser->homedir);
    };
   


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
    int fd = open("passwd", O_RDONLY);
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
    mkdir(dir);

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

int main(int argc, char* argv[])
{
    printf(1, "Welcome to sastagrub.c\n\n");
    
    createUserArray();
	while(authenticateUser(&arrIndex)==0);

	if(chdir(userArray[arrIndex]->homedir)<0){
        printf(1, "chdir failed\n");
    }
    
	setuid(userArray[arrIndex]->uid);
    seteuid(userArray[arrIndex]->uid);
    setegid(userArray[arrIndex]->gid);
    // setegid(userArray[arrIndex].
	// exec("/bin/sh", argv);
	


    // this is going to be useful for the su command. We will have to change the proc, this is how we do it!
//     void inituid(int uid)
// {
//   struct proc *p = myproc();
//   p->uid = uid;
//   p->euid = uid;
// }
	exec("sh", argv);
	exit();

}
