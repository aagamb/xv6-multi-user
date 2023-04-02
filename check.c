#include "types.h"
#include "stat.h"
#include "user.h"

int main(){

	char dest[50];
	char source[] = "Hello World!\n";
	strcpy(dest, source);
	printf(1, "%s", dest);
	exit();
}
