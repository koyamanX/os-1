#ifndef UNITSTD_H
#define UNITSTD_H

#include <sys/types.h>
#include <stddef.h>

ssize_t write(int fd, const void *buf, size_t count);
int dup(int fildes);

#endif
