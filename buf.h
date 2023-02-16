#ifndef BUF_H
#define BUF_H

#include "types.h"

#define BUFSIZ 1024

void binit(void);
struct buf *bread(u32 dev, u64 blkno);
int bwrite(struct buf *bp);
int bflush(u32 dev);

struct buf {
	int valid;
	u32 dev;
	u64 blkno;
	char data[BUFSIZ];
};

extern struct buf *blist;

#define VIRTIO_BLK 0

#endif
