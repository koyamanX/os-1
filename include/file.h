#ifndef FILE_H
#define FILE_H

#include <fs.h>
#include <sys/types.h>

struct file {
    u32 flags;
    u64 count;
    u64 offset;
    struct inode *ip;
};

#define NFILE 128

extern struct file file[];
struct file *falloc(void);
int ufalloc(void);
void openi(struct inode *ip);
void closei(struct inode *ip);

#endif
