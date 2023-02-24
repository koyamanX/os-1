#ifndef DEVSW_H
#define DEVSW_H

#include <stdint.h>
#include <sys/types.h>

typedef struct dev_t {
	uint32_t major;
	uint32_t minor;
} dev_t;

struct bdevsw {
	int (*open)(void);
	int (*close)(void);
	int (*strategy)(char *, u64, u8);
};

struct mount {
	dev_t dev;			// device
	char *sb;			// pointer to superblock
	struct inode *ip;	// pointer to mounted i-node
};

extern struct bdevsw bdevsw[];
extern struct mount mount[];
extern dev_t rootdev;

#define NBDEVSW 4
#define NMOUNT 32

#endif
