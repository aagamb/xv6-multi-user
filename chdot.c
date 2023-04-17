  #include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf(2, "Usage: chdotuid <inode-number> <uid>\n");
    exit();
  }

  int inum = atoi(argv[1]);
  int uid = atoi(argv[2]);

  if(chdotuid(inum, uid) < 0){
    printf(2, "chdotuid failed\n");
    exit();
  }
  printf(1, "done");
  exit();
}

