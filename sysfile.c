#define O_NO_DEREF  0x004
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

#define LINK_LIMIT 50
#define USER_R_BIT   1
#define USER_W_BIT   2
#define USER_X_BIT   3
#define GROUP_R_BIT  4
#define GROUP_W_BIT  5
#define GROUP_X_BIT  6
#define OTHER_R_BIT  7
#define OTHER_W_BIT  8
#define OTHER_X_BIT  9

void integer_to_binary(unsigned int in, int count, int* out)
{
    unsigned int mask = 1U << (count-1);
    int i;
    for (i = 0; i < count; i++) {
        out[i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=myproc()->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;
  struct proc *curproc = myproc();

  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd] == 0){
      curproc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

int
sys_dup(void)
{
  struct file *f;
  int fd;

  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}

int
sys_read(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return fileread(f, p, n);
}

int
sys_write(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return filewrite(f, p, n);
}

int
sys_close(void)
{
  int fd;
  struct file *f;

  if(argfd(0, &fd, &f) < 0)
    return -1;
  myproc()->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

int
sys_fstat(void)
{
  struct file *f;
  struct stat *st;

  if(argfd(0, 0, &f) < 0 || argptr(1, (void*)&st, sizeof(*st)) < 0)
    return -1;
 if(f->ip->type == T_SYMLINK) {
    struct inode *resolved_ip;
    char buf[100];

    ilock(f->ip);
    read_symlink(f->ip, buf, sizeof(buf));
    iunlock(f->ip);

    begin_op();
    resolved_ip = namei(buf, 1);
    if(resolved_ip == 0) {
      end_op();
      return -1;
    }
    ilock(resolved_ip);
    stati(resolved_ip, st);
    iunlockput(resolved_ip);
    end_op();
  } else {
    ilock(f->ip);
    stati(f->ip, st);
    iunlock(f->ip);
  }
  return 0;
}


// Create the path new as a link to the same inode as old.
int
sys_link(void)
{
  char name[DIRSIZ], *new, *old;
  struct inode *dp, *ip;

  if(argstr(0, &old) < 0 || argstr(1, &new) < 0)
    return -1;

  begin_op();
  if((ip = namei(old,1)) == 0){
    end_op();
    return -1;
  }

  ilock(ip);
  if(ip->type == T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  end_op();

  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  end_op();
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
static int
isdirempty(struct inode *dp)
{
  int off;
  struct dirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      panic("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

//PAGEBREAK!
int
sys_unlink(void)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ], *path;
  uint off;

  if(argstr(0, &path) < 0)
    return -1;

  begin_op();
  if((dp = nameiparent(path, name)) == 0){
    end_op();
    return -1;
  }

  ilock(dp);

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  if((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  if(ip->nlink < 1)
    panic("unlink: nlink < 1");
        if(ip->type == T_SYMLINK) {
    goto remove_link;
  }
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    panic("unlink: writei");
  if(ip->type == T_DIR){
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);
  remove_link:
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  end_op();

  return 0;

bad:
  iunlockput(dp);
  end_op();
  return -1;
}

// int checkWritePermission(char* path) {
//     // If the current user can access the parent, they can access the files inside it
//     char parent[16];
//     struct inode* inodeParent = nameiparent(path, parent);

//     // int uidParent = inodeParent->uid;
//     // int gidParent = inodeParent->gid;
//     int modeParent = inodeParent->mode;

//     // Check the read permission bits for user, group, and others
//     // int modeReading = 0b100100100;

    

//     if (myproc()->euid == 0) {
//         // Root user can read all files
//         return 1;
//     } else if ((modeParent & 0200)) {
//         // File owner has read permission
//         return 1;
//         // gidParent == myproc()->egid && 
//     } else if ((modeParent & 0020)) {
//         // Group has read permission
//         return 1;
//     } else if (modeParent & 0002) {
//         // Others have read permission
//         return 1;
//     }
//     return -1;
// }

// int checkReadPermission(char* path) {
//     // If the current user can access the parent, they can access the files inside it
//     char parent[16];
//     struct inode* inodeParent = nameiparent(path, parent);

//     int uidParent = inodeParent->uid;
//     int modeParent = inodeParent->mode;

//     // Check the read permission bits for user, group, and others
//     // int modeReading = 0b100100100;

//     if (myproc()->euid == 0) {
//         // Root user can read all files
//         return 1;
//     } else if (uidParent == myproc()->euid && (modeParent & 0400)) {
//         // File owner has read permission
//         return 1;
//     } else if (modeParent & 0004) {
//         // Others have read permission
//         return 1;
//     }
//     return -1;
// }

// int checkExecutePermission(char* path) {
//     // If the current user can access the parent, they can access the files inside it
//     char parent[16];
//     struct inode* inodeParent = nameiparent(path, parent);

//     int uidParent = inodeParent->uid;
//     int modeParent = inodeParent->mode;

  
//     if (myproc()->euid == 0) {
//         // Root user can execute all files
//         return 1;
//     } else if (uidParent == myproc()->euid && (modeParent & 0100)) {
//         // File owner has execute permission
//         return 1;
//     } else if (modeParent & 0001) {
//         // Others have execute permission
//         return 1;
//     }
//     return -1;
// }

static struct inode*
create(char *path, short type, short major, short minor, int uid, int mode)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  // if(checkWritePermission(path)<0){
  //   // printf(1, "checkWritePermission Failed\n");
  //   panic("checkWritePermission failed\n");
  //   return NULL;
  // }
    

  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);

  if((ip = dirlookup(dp, name, 0)) != 0){
    iunlockput(dp);
    ilock(ip);
    if((type == T_FILE && ip->type == T_FILE)|| ip->type == T_DEV)
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(dp->dev, type,uid,mode)) == 0)
    panic("create: ialloc");

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  ip->mode = 00644;
  iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      panic("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0)
    panic("create: dirlink");

  iunlockput(dp);

  return ip;
}

int
sys_open(void)
{
  char *path;
  int fd, omode;
  struct file *f;
  struct inode *ip;
  char buf[100];
  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;
 
  // if (myproc()->euid !=0){
  //   if( omode == O_WRONLY || omode == O_RDWR || omode == O_CREATE){
  //     if(checkWritePermission(path)<0){
  //       return -1;
  //     }
  //   }
      
    
  //   if(omode == O_RDONLY || omode == O_RDWR){
  //     if(checkReadPermission(path)<0){
  //       return -1;
  //     }
  //   }
    
  // }

  begin_op();

  if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0, myproc()->uid, 00644);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path,1)) == 0){
      end_op();
      return -1;
    }
    ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
     if(omode & O_NO_DEREF)
        goto noderef;
      iunlockput(ip);
      end_op();
      return -1;
    }
    noderef:
    if(ip->type == T_SYMLINK && (omode & O_NO_DEREF)==0 )
    {
      iunlock(ip);
      int res = read_symlink(path, buf, 100);
      if(res < 0) {
        end_op();
        return -1;
      }
      ip = namei(buf,1);
      if (ip == 0) {
         end_op();
        return -1;
      }
      ilock(ip);
    }

    if(ip->type == T_SYMLINK && (omode & O_NO_DEREF))
    {
      iunlock(ip);
      ip = namei(path,0);
      ilock(ip);
    }
    
    }

    int bits[10];
        integer_to_binary(ip -> mode, 10, bits);

        struct proc *currproc = myproc();
        int fileowner = ip -> uid == currproc -> euid;
        int other = (fileowner != 1);
        int invalid = 0;
        
        if (omode == O_RDWR) {
                if (fileowner) {
                        if (!bits[USER_R_BIT] || !bits[USER_W_BIT])
                                invalid = 1;
                } 
                else if (other) {
                        if (!bits[OTHER_R_BIT])
                                invalid = 1;
                }
        } else if (omode == O_RDONLY) {
                if (fileowner) {
                        if (!bits[USER_R_BIT])
                                invalid = 1;
                } 
                else if (other) {
                        if (!bits[OTHER_R_BIT])
                                invalid = 1;
                }
        } else if (omode == O_WRONLY) {
                if (fileowner) {
                        if (!bits[USER_W_BIT])
                                invalid = 1;
                }
                else if (other) {
                        if (!bits[OTHER_W_BIT])
                                invalid = 1;
                }
	}

        // If we're root, invalid is not important
        invalid = currproc -> euid == 0 ? 0 : invalid;

        if (invalid) {
                cprintf("%s: Permission denied\n", path);
                iunlockput(ip);
                end_op();
                return -1;
        }

  
    
  

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    iunlockput(ip);
    end_op();
    return -1;
  }
  iunlock(ip);
  end_op();

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}

int
sys_mkdir(void)
{
  char *path;
  struct inode *ip;

  begin_op();
  if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0, myproc()->uid, 00644)) == 0){
    // panic("error here");
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_mknod(void)
{
  struct inode *ip;
  struct inode *parent;
  char name[14];
  char *path;
  int major, minor;

  int bits[10];
  parent = nameiparent(path, name);
	integer_to_binary(parent -> mode, 10, bits);

	int fileowner = parent -> uid == myproc() -> euid;
	int groupmember = 0;

	int other = fileowner != 1 && groupmember != 1;
	int invalid = 0;

	if (fileowner) {
		if (!bits[USER_W_BIT])
			invalid = 1;
	} else if (groupmember) {
		if (!bits[GROUP_W_BIT])
			invalid = 1;
	}
	else if (other) {
		if (!bits[OTHER_W_BIT])
			invalid = 1;
	}

	// If we're root, invalid is not important
	invalid = myproc() -> euid == 0 ? 0 : invalid;

	if (invalid) {
		cprintf("%s: Permission denied\n", path);
		iunlockput(ip);
		end_op();
		return -1;
	}


  begin_op();
  if((argstr(0, &path)) < 0 ||
     argint(1, &major) < 0 ||
     argint(2, &minor) < 0 ||
     (ip = create(path, T_DEV, major, minor, myproc()->uid, 00644)) == 0){
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}

int
sys_chdir(void)
{
  char *path;
  struct inode *ip;
  struct proc *curproc = myproc();
  char path_name[LINK_LIMIT];
  
  begin_op();
  if(argstr(0, &path) < 0 || (ip = namei(path,1)) == 0){
    end_op();
    return -1;
  }
    if(read_symlink(path, path_name, LINK_LIMIT) == 0) 
  {
    if((ip = namei(path_name, 1)) == 0) 
    {
      end_op();
      return -1;
    }
  }
  else if((ip = namei(path, 1) )== 0) 
  {
    end_op();
    return -1;
  }

  

  
  // if(checkExecutePermission(path)<0){
  //   end_op();
  //   cprintf("My function failed\n");
  //   return -1;
  // }
  
  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  int bits[10];
	integer_to_binary(ip -> mode, 10, bits);
	int hasperm = 0;

	int fileowner = ip -> uid == curproc -> euid;
	int other = fileowner != 1;
  cprintf("Fileowner, other: %d, %d\n", fileowner, other);
	int invalid = 0;
	if (fileowner) {
		if (ip->mode && 0100)
			invalid = 0;
	} 
	else if (other) {
		if (ip->mode && 0001)
			invalid = 0;
	}

	// If we're root, invalid is not important
	invalid = curproc -> euid == 0 ? 0 : invalid;

	if (invalid) {
		cprintf("%s: Permission denied\n", path);
		iunlockput(ip);
		end_op();
		return -1;
	}


  iunlock(ip);
  iput(curproc->cwd);
  end_op();
  curproc->cwd = ip;
  return 0;
}

int
sys_exec(void)
{
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      myproc()->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}

int
sys_login(void)
{
  char *username;
  char *password;

  // Get arguments from user stack
  if (argstr(0, &username) < 0 || argstr(1, &password) < 0) {
    return -1;
  }

  // TODO: Authenticate the user
  // Read the password file from '/etc' directory
  // Verify the credentials
//int fd = open("/passwd", O_RDONLY);
 // if (fd < 0) {
 //   return -1;
 // }

int authenticated = 1;

  if (authenticated) {
    return 0;
  } else {
    return -1;
  }
}

int
create_symlink(const char* oldpath , const char* newpath)
{
  struct file *f;
  struct inode *ip;

  begin_op();

  if((ip = create((char*)newpath, T_SYMLINK, 0, 0, myproc()->uid, 00644)) == 0)
  {
    end_op();
    return -1;
  }

  end_op();

  if((f = filealloc()) == 0)
  {
    if(f)
      fileclose(f);
    iunlockput(ip);
    return -1;
  }

  if(strlen(oldpath) > LINK_LIMIT)
    panic("symlink: new path is too long");
  safestrcpy((char*)ip->addrs, oldpath, LINK_LIMIT);
  iunlock(ip);

  f->ip = ip;
  f->off = 0;
  f->readable = 1;
  f->writable = 0;
  return 0;
}

int
read_symlink(const char* pathname, char* buf, size_t bufsize)
{

  if(strlen(pathname) > bufsize)
  {
   
    return -1;
  }

  struct inode * ip;
  if ((ip = namei((char*)pathname, 1)) == 0)  // checks if the path exists
  {
    return -1;
  }
  
  ilock(ip);
  if(ip->type != T_SYMLINK)
  {
    iunlock(ip);
    return -1;
  }
  
  char buf_temp[LINK_LIMIT];
  safestrcpy(buf_temp,(char*)ip->addrs, LINK_LIMIT);
  struct inode * ip_next;
  if ((ip_next = namei((char*)buf_temp, 1)) > 0)  // checks if the path exists
  {
      if(ip_next->type != T_SYMLINK)
      {
        safestrcpy(buf,(char*)ip->addrs, bufsize);
        iunlock(ip);
        return 0;
      }
      else
      {
          iunlock(ip);
          return read_symlink(buf_temp,buf, bufsize);
      }
  }
  else
  {
    iunlock(ip);
   // cprintf("file doesnt exist");
    return -1;
  }
  
}

int sys_chmod(void) {
    char *path;
    int mode;

    if (argstr(0, &path) < 0 || argint(1, &mode) < 0) {
        return -1;
    }

    struct inode *ip;
    begin_op();
    if ((ip = namei(path,0)) == 0) {
        end_op();
        return -1;
    }
    ilock(ip);

    // Check if the user is the owner of the file or the superuser
    if (ip->uid != myproc()->uid && myproc()->uid != 0) {
        iunlockput(ip);
        end_op();
        return -1;
    }

    // Clear the previous permission bits, setuid, and setgid bits, and apply the new mode
    ip->mode = (ip->mode & ~(0777 | S_ISUID | S_ISGID)) | mode;
    iupdate(ip);
    iunlockput(ip);
    end_op();
    return 0;
}

