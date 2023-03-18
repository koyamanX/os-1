#ifndef DEVSW_H
#define DEVSW_H

#include <buf.h>
#include <stdint.h>
#include <sys/types.h>

struct bdevsw {
    int (*open)(void);
    int (*close)(void);
    int (*strategy)(char *, u64, u8);
    struct buf *bactivelist;
};

struct cdevsw {
    int (*open)(void);
    int (*close)(void);
    int (*read)(void);
    int (*write)(int c);
    int (*sgtty)(void);
};

struct mount {
    dev_t dev;               // device
    struct super_block *sb;  // pointer to superblock
    struct inode *ip;        // pointer to mounted i-node
};

extern struct bdevsw bdevsw[];
extern struct cdevsw cdevsw[];
extern struct mount mount[];
extern dev_t rootdev;

#define NBDEVSW 4
#define NCDEVSW 4
#define NMOUNT 32

#endif
