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
#include "buf.h"

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

int isHomeThere(const char *path) {
  const char *home = "/home/";

  // Iterate through the path and look for the "/home/" substring
  for (int i = 0; path[i] != '\0'; i++) {
    int j;
    for (j = 0; home[j] != '\0' && path[i + j] != '\0' && path[i + j] == home[j]; j++) {
      // Intentionally left empty
    }

    if (home[j] == '\0') {
      return 1;
    }
  }

  return 0;
}


void integer_to_binary(int n, int length, int *out) {
  for (int i = length - 1; i >= 0; i--) {
    out[i] = n % 2;
    n /= 2;
  }
}

int has_permission(struct inode *ip, int permission_bit) {
  int bits[10];
  integer_to_binary(ip->mode, 10, bits);
  
  struct proc *currproc = myproc();
  int fileowner = ip->uid == currproc->euid;
  int other = (fileowner != 1);
  
  if (fileowner) {
    return bits[permission_bit];
  } else if (other) {
    return bits[permission_bit + 6];
  }
  
  return 0;
}

int
has_requested_permission(struct inode *ip, int mask)
{
  int mode = ip->mode;
  // cprintf("\tip->mode=%d\n", ip->mode);

  if (myproc()->uid == 0) {
    return 1; // root user has full permissions
  }

  if (myproc()->uid == ip->uid) {
    mask <<= 6;
  }
  // } else if (myproc()->gid == ip->gid) {
  //   mask <<= 3;
  // }

  if ((mode & mask) == mask) {
    // cprintf("mode & mask: %d\n", mode&mask);
    return 1;
  }
  return 0;
}


// void integer_to_binary(unsigned int in, int count, int* out)
// {
//     unsigned int mask = 1U << (count-1);
//     int i;
//     for (i = 0; i < count; i++) {
//         out[i] = (in & mask) ? 1 : 0;
//         in <<= 1;
//     }
// }

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
  ip->mode = 0644;
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
 

  begin_op(); 

  // cprintf("omode is: %d\n", omode);
  // int isHomeThereNum  = isHomeThere(path);
  // cprintf("temp is: %d %s\n", isHomeThereNum, path);
  if(omode & O_CREATE){
    ip = create(path, T_FILE, 0, 0, myproc()->uid, 0644);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {

    ip = namei(path, 0);

    // cprintf("\tip->mode: %d\tip->mode & 06000: %d\tip->uid%d\n", ip->mode, (ip->mode & 06000), ip->uid);

     // cprintf("ip->uid , myproc()->euid: %d %d\n", ip->uid, myproc()->euid);
  //  if ((ip->uid != myproc()->euid)){
  //   if (!((ip->mode & 04000) || (ip->mode & 02000))){
  //     end_op();
  //     return -1;
  //   }
  //  } 
      
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

    if (ip->type == T_DIR && omode != O_RDONLY) {
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
  char *path = "\0";
  int major, minor;

  int bits[10];
  parent = nameiparent(path, name);
	integer_to_binary(parent -> mode, 10, bits);

	// int fileowner = parent -> uid == myproc() -> euid;
	// int groupmember = 0;

	int invalid = 0;


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

  

  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  int bits[10];
	integer_to_binary(ip -> mode, 10, bits);

	int fileowner = ip -> uid == curproc -> euid;
	int other = fileowner != 1;
  //cprintf("Fileowner, other: %d, %d\n", fileowner, other);
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

  //struct inode *ip;
 // if ((ip = namei(path,1)) == 0) {
  //  cprintf("here\n");
   // return -1;
 // }
  
  // ilock(ip);
  // if (!has_permission(ip, USER_X_BIT)) {
  //   cprintf("%s: Permission denied Execute\n", path);
  //   iunlockput(ip);
  //   return -1;
  // }
  // iunlock(ip);


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

int sys_setuid(void) {
  int uid;

  if (argint(0, &uid) < 0) {
    return -1;
  }
   struct proc *curproc = myproc();
   struct inode *ip = myproc()->exec_inode;

  // ilock(ip);
   if (curproc->uid == 0 || (ip->mode & 04000) || (ip->mode & 02000)) {
   curproc->uid = uid;
  //  iunlock(ip);
   return 0;
 }
  // iunlock(ip);
  return 0;
}

int
sys_seteuid(void)
{
  int euid;
  if(argint(0, &euid) < 0)
    return -1;

 struct proc *curproc = myproc();
 struct inode *ip = myproc()->exec_inode;

//  cprintf("inode mode: %d\n", ip->mode);

  ilock(ip);
 if (curproc->uid == 0 || (ip->mode & 04000) || (ip->mode & 02000)) {
   curproc->euid = euid;
    iunlock(ip);
   return 0;
 }
  iunlock(ip);

 return -1;
}

int
sys_setegid(void)
{
  int egid;
  if(argint(0, &egid) < 0)
    return -1;

 struct proc *curproc = myproc();
 struct inode *ip = myproc()->exec_inode;

//  cprintf("inode mode: %d\n", ip->mode);

  ilock(ip);
 if (curproc->uid == 0 || (ip->mode & 04000) || (ip->mode & 02000)) {
   curproc->egid = egid;
    iunlock(ip);
   return 0;
 }
  iunlock(ip);

 return -1;
}

int
sys_setreuid(void)
{
  int ruid, euid;
  if(argint(0, &ruid) < 0 || argint(1, &euid) < 0)
    return -1;

  struct proc *curproc = myproc();

  struct inode *ip = myproc()->exec_inode;

    // ilock(ip);
  if (curproc->uid == 0 || (ip->mode & 04000) || (ip->mode & 02000)) {
    curproc->uid = ruid;
    curproc->euid = euid;
    // iunlock(ip);
    return 0;
  }
    // iunlock(ip);
    
    return -1;
  }

int
sys_setregid(void)
{
  int rgid, egid;
  if(argint(0, &rgid) < 0 || argint(1, &egid) < 0)
    return -1;

  struct proc *curproc = myproc();

  struct inode *ip = myproc()->exec_inode;

    // ilock(ip);
  if (curproc->uid == 0 || (ip->mode & 04000) || (ip->mode & 02000)) {
    curproc->gid = rgid;
    curproc->egid = egid;
    
    // iunlock(ip);
    return 0;
  }
    // iunlock(ip);
    
    return -1;
}

int
chdotuid(int inum, int uid)
{
  struct inode *ip;
  struct dirent de;
  struct inode *dot_inode;

  begin_op();
  ip = get_inode_by_num(ROOTDEV, inum);

  if(ip == 0){
    end_op();
    return -1;
  }

  ilock(ip);

  if(ip->type != T_DIR){
    iunlockput(ip);
    end_op();
    return -1;
  }

  if(readi(ip, (char*)&de, 0, sizeof(de)) != sizeof(de)){
    iunlockput(ip);
    end_op();
    return -1;
  }

  dot_inode = get_inode_by_num(ROOTDEV, de.inum);
  ilock(dot_inode);

  dot_inode->uid = uid;
  iupdate(dot_inode);

  iunlockput(ip);
  iunlockput(dot_inode);
  end_op();

  return 0;
}

int
sys_chdotuid(void)
{
  int inum;
  int uid;

  if(argint(0, &inum) < 0 || argint(1, &uid) < 0)
    return -1;

  return chdotuid(inum, uid);
}

int sys_mkdir2(void)
{
  char *path;
  struct inode *ip;
  int uid;

  begin_op();

  if (argint(1, &uid) <0){
    end_op();
    return -1;
  }
  if(argstr(0, &path) < 0 || (ip = create(path, T_DIR, 0, 0, uid, 00644)) == 0){
    // panic("error here");
    end_op();
    return -1;
  }
  iunlockput(ip);
  end_op();
  return 0;
}
