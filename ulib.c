#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}



// char* strcat(char* dest, const char* source)
// {
// 	char* ptr = dest + strlen(dest);
// 	while (*source != '\0')
// 		*ptr++ = *source++;
// 	*ptr = '\0';
// 	return dest;
// }

char *strtok(char *str, const char *delim) {
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

void strncpy(char *dest, const char *src, int n) {
    for (int i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (int i = strlen(src); i < n; i++) {
        dest[i] = '\0';
    }
}

char* strcat(char* dest, const char* src) { 
    char* p = dest;
    while (*p != '\0') {
        p++;
    }
    while (*src != '\0') {
        *p = *src;
        p++;
        src++;
    }
    *p = '\0'; 
    return dest;
}

void reverse(char* str, int len) {
    int i = 0, j = len - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


void itoa(int num, char* str) {
    int i = 0;
    int sign = num < 0 ? -1 : 1;
    if (sign == -1) {
        num = -num;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (sign == -1) {
        str[i++] = '-';
    }

    str[i] = '\0';
    reverse(str, i);
}

