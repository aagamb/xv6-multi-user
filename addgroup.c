#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "myuser.h"

#define MAX_GROUPS 128
#define MAX_GROUPNAME_LEN 32
#define MAX_LINE_LEN 64

struct group {
  int gid;
  char groupname[MAX_GROUPNAME_LEN];
};


int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf(1, "Usage: %s gid groupname\n", argv[0]);
    exit();
  }

  int gid = atoi(argv[1]);
  char *groupname = argv[2];

  struct group* groups[MAX_GROUPS];
  for (int y=0; y<MAX_GROUPS; y++){
    groups[y] = (struct group*)malloc(sizeof(struct group));
  }
  int group_count = 0;

  int fd = open("/group", O_RDONLY);
  if (fd < 0) {
    printf(1, "Error: Cannot open /group\n");
    exit();
  }

  char line[MAX_LINE_LEN];
  int line_pos = 0;

  char c;
  while (read(fd, &c, 1) > 0) {
  if (c == '\n' || c == '\0') {
    line[line_pos] = '\0';

    char *groupname_str = strtok(line, ":");
    strtok(NULL, ":"); 
    char *gid_str = strtok(NULL, ":");
    
    // printf(1, "groupname_str: %s, gid_str: %s\n", groupname_str, gid_str);

    groups[group_count]->gid = atoi(gid_str);
    strncpy(groups[group_count]->groupname, groupname_str, MAX_GROUPNAME_LEN - 1);
    group_count++;

    line_pos = 0;
  } else {
    line[line_pos++] = c;
  }
}

close(fd);

for (int i = 0; i < group_count; i++) {
  if (groups[i]->gid == gid) {
    printf(1, "Error: gid %d already exists\n", gid);
    exit();
  }

  if (strcmp(groups[i]->groupname, groupname) == 0) {
    printf(1, "Error: groupname %s already exists\n", groupname);
    exit();
  }
}

  fd = open("/group", O_WRONLY);
  if (fd < 0) {
    printf(1, "Error: Cannot open /etc/group for writing\n");
    exit();
  }

  char string_to_append[MAX_LINE_LEN];
  char gid_str[MAX_LINE_LEN];
    itoa(gid, gid_str);
    strcat(string_to_append, groupname);
    strcat(string_to_append, ":x:");
    strcat(string_to_append, gid_str);
    strcat(string_to_append, ":");
    strcat(string_to_append, "\n");

  close(fd);

   fd = open("/group", O_RDONLY);
    if (fd < 0) {
        printf(2, "Error: cannot open file %s\n", "/group");
        exit();
    }

    char buffer[1024];
    int bytes_read = 0;
    int total_bytes_read = 0;

    while ((bytes_read = read(fd, buffer + total_bytes_read, 1024 - total_bytes_read - 1)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read == 1024 - 1) {
            printf(2, "Error: file too large to read into buffer\n");
            close(fd);
            exit();
        }
    }

    close(fd);

    if (bytes_read < 0) {
        printf(2, "Error: cannot read file %s\n", "/group");
        exit();
    }

    int append_len = strlen(string_to_append);
    memmove(buffer + total_bytes_read, string_to_append, append_len);
    total_bytes_read += append_len;

    fd = open("/group", O_WRONLY | O_CREATE);
    if (fd < 0) {
        printf(2, "Error: cannot open file %s\n", "/group");
        exit();
    }

    int bytes_written = write(fd, buffer, total_bytes_read);
    if (bytes_written != total_bytes_read) {
        printf(2, "Error: cannot write to file %s\n", "/group");
    }

    close(fd);

  printf(1, "Group added successfully: gid %d, groupname %s\n", gid, groupname);
  exit();
    // return 0;
}

