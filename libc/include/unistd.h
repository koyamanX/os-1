#ifndef UNITSTD_H
#define UNITSTD_H

#include <sys/types.h>
#include <stddef.h>

ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, const void *buf, size_t count);
int dup(int fildes);
int fork(void);
int exec(const char *pathname, const char *argv[]);

#endif
