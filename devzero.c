#include "types.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

#define ZEROBUFFSIZE 512
// static char zerobuff[ZEROBUFFSIZE];


int devzeroread(struct inode *ip, char *buf, int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		buf[i]=0;
	}
	return i;
}

int devzerowrite(struct inode *ip, char *buf, int n)
{
	/*
	int i;
	for(i=0;i<n && i < ZEROBUFFSIZE-1;i++)
	{
		//cprintf("%d\n",i);
		zerobuff[i]=buf[i];
	} */
	return n;

}

int devzeroinit(void)
{
	devsw[DEVZERO].read = devzeroread;
	devsw[DEVZERO].write = devzerowrite;
	return 0;
}

