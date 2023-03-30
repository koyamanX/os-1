#ifndef BUF_H
#define BUF_H

#include <devsw.h>
#include <sys/types.h>

#define BUFSIZ 1024

void binit(void);
struct buf *bread(dev_t dev, u64 blkno);
int bwrite(struct buf *bp);
int bflush(dev_t dev);

struct buf {
    int valid;
    dev_t dev;
    u64 blkno;
    char data[BUFSIZ];
};

extern struct buf *blist;

#endif
