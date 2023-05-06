#ifndef _DEVSW_H
#define _DEVSW_H

#include <buf.h>
#include <riscv.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * @brief Block device switch table
 * @details The bdevsw table is used to manage block devices.
 */
struct bdevsw {
    int (*open)(void);
    int (*close)(void);
    int (*strategy)(char *, u64, u8);
    struct buf *bactivelist;
};

/**
 * @brief Character device switch table
 * @details The cdevsw table is used to manage character devices.
 */
struct cdevsw {
    int (*open)(void);
    int (*close)(void);
    int (*read)(void);
    int (*write)(int c);
};

/**
 * @brief Mount table
 * @details The mount table is used to manage mounted file systems.
 */
struct mount {
    dev_t dev;               // device
    struct super_block *sb;  // pointer to superblock
    struct inode *ip;        // pointer to mounted i-node
};

extern struct bdevsw bdevsw[];
extern struct cdevsw cdevsw[];
extern struct mount mount[];
extern dev_t rootdev;  //!< root device

#define NBDEVSW 4  //!< number of block devices
#define NCDEVSW 4  //!< number of character devices
#define NMOUNT 32  //!< number of mount points

#endif /* _DEVSW_H */
