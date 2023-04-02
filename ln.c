#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf(2, "Usage (hard): ln old new\n");
    printf(2, "-OR-\n");
    printf(2, "Usage (symbolic): ln −s old_path new_path\n");
    exit();
  }
  if(argc == 3) {
    if(link(argv[1], argv[2]) < 0)
      printf(2, "hard link %s %s: failed\n", argv[1], argv[2]);
    exit();
  }

  if(argc == 4) {
    if(symlink(argv[2], argv[3]) < 0)
      printf(2, "symlink %s %s: failed\n", argv[1], argv[2]);
    exit();
  }

  exit();
  
}
