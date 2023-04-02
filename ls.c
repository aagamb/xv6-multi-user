#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
// #include "file.h"

void convert_permissions_to_str(int permissions, char *output_str) {
    const char *perm_chars = "rwx";

    for (int i = 0; i < 3; ++i) {
        int perm_set = (permissions >> (3 * (2 - i))) & 0b111;

        for (int j = 0; j < 3; ++j) {
            output_str[i * 3 + j] = (perm_set & (1 << (2 - j))) ? perm_chars[j] : '-';
        }
    }

    output_str[9] = '\0';
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';

    printf(1, "The uid, euid: %d, %d \n", getuid(), geteuid());
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
	char temp[100];
	convert_permissions_to_str(st.mode, temp);
  
	printf(1, "%d %s:\t", getuid(), temp);

      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  int temp = geteuid();
  seteuid(0);
  for(i=1; i<argc; i++)
    ls(argv[i]);
  seteuid(temp);
  exit();
}
