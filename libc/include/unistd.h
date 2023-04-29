#ifndef UNITSTD_H
#define UNITSTD_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, const void *buf, size_t count);
int dup(int fildes);
int fork(void);
int execv(const char *pathname, const char *argv[]);
void _exit(int status);
int close(int fd);
int link(const char *path1, const char *path2);
int truncate(const char *path, off_t length);
void *sbrk(intptr_t increment);
int brk(void *addr);

#endif
