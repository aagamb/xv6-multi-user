#ifndef _SOFTLINK_H_
#define _SOFTLINK_H_

extern int create_symlink(const char* oldpath, const char* newpath);
extern int read_symlink(const char* pathname, char* buf, size_t bufsize);
extern int strncmp(const char *s1, const char *s2, size_t n);

#endif // _SOFTLINK_H_

