#ifndef FILE_H
#define FILE_H

#include <sys/types.h>
#include <fs.h>

struct file {
	u32 flags;
	u64 count;
	u64 offset[2];
	struct inode *ip;
};

#define NFILE 128

extern struct file file[];
struct file *falloc(void);
int ufalloc(void);

#endif
