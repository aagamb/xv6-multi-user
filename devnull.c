#include "types.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"


#define NULLBUFFSIZE 512
static char nullbuff[NULLBUFFSIZE];


int devnullread(struct inode *ip, char *buf, int n)
{
	return 0;
}

int devnullwrite(struct inode *ip, char *buf, int n)
{
        /*
	int i;
	for(i=0;i<n && i < NULLBUFFSIZE-1;i++)
	{
		//cprintf("%d\n",i);
		nullbuff[i]=buf[i];
	} */
	return n;

}

int devnullinit(void)
{
	devsw[DEVNULL].read = devnullread;
	devsw[DEVNULL].write = devnullwrite;
}

