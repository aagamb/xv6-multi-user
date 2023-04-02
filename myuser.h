#include "types.h"
//#include "stat.h"
#include "fcntl.h"
#include "user.h"



struct user{
        int uid;
        int gid;
	char personName[100];
        char username[100];
	char homedir[100];
        char password[100];
        char shell[100];
};

extern struct user* userArray[100];
