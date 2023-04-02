#include "types.h"
#include "stat.h"
#include "user.h"
 
int 
main(void) {
    printf(1, "return val of system call is %d\n", getuid());
    printf(1, "orig euid: %d", geteuid());
	seteuid(30);
    printf(1, "neweuid : %d", geteuid());
    exit();
 }
