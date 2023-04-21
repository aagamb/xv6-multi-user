struct stat;
struct rtcdate;
#include <stddef.h>

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int login(char *username, char *password); 
int setuid(int);
int getuid();
int geteuid();
int seteuid(int);
int setegid(int);
int setreuid(int ruid, int euid);
int setregid(int rgid, int egid);
int symlink(const char*, const char*);
int readlink(const char*, char*, size_t);
int chmod(const char*, int mode);
int chdotuid(int inum, int uid);
int mkdir2(const char*, int uid);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
char *strtok(char *str, const char *delim);
char* strcat(char* dest, const char* src); 
void itoa(int num, char* str);
void strncpy(char *dest, const char *src, int n);
