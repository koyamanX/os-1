#ifndef FCNTL_H
#define FCNTL_H

#define O_CREAT        00100
#define O_TRUNC        01000
#define O_RDONLY           0
#define O_WRONLY           1
#define O_RDWR             2

#include <sys/types.h>

int open(const char *pathname, int flags, mode_t mode);
int creat(const char *pathname, mode_t mode);

#endif
