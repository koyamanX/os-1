#ifndef FILE_H
#define FILE_H

#include <types.h>
#include <fs.h>

struct file {
	u32 flags;
	u64 count;
	u64 offset[2];
	struct inode *ip;
};

#define FREAD	0x1
#define FWRITE	0x2
#define FPIPE	0x4

#define NFILE 128

extern struct file file[];
struct file *falloc(void);

#endif
