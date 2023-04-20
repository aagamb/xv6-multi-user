#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "mylib.h"
#include "x86.h"


char *strtok22(char *str, const char *delim) {
  static char *last;
  if (str != NULL) {
    last = str;
  }
  if (last == NULL || *last == '\0') {
    return NULL;
  }
  char *token = last;
  while (*last != '\0') {
    if (strchr(delim, *last) != NULL) {
      *last++ = '\0';
      break;
    }
    last++;
  }
  return token;
}
	
