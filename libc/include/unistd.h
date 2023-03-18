#ifndef UNITSTD_H
#define UNITSTD_H

#include <stddef.h>
#include <sys/types.h>

ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, const void *buf, size_t count);
int dup(int fildes);
int fork(void);
int exec(const char *pathname, const char *argv[]);
void _exit(int status);
int close(int fd);

#endif
