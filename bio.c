#include "buf.h"
#include "vm.h"
#include "virtio.h"

struct buf *blist;

void binit(void) {
	blist = (struct buf *)kalloc();	
	blist->valid = 0;
}

struct buf *bread(u32 dev, u64 blkno) {
	if(blist->valid && blist->blkno == blkno) {
		return blist;
	}
	if(blist->valid) {
		bwrite(blist);
		blist->valid = 0;
	}
	blist->valid = 1;
	blist->dev = dev;
	blist->blkno = blkno;
	virtio_req(&blist->data[0], blist->blkno+0, 0);
	virtio_req(&blist->data[512], blist->blkno+1, 0);
	
	return blist;
}

int bwrite(struct buf *bp) {
	if(!bp->valid) {
		return -1;
	}
	virtio_req(&bp->data[0], bp->blkno+0, 1);
	virtio_req(&bp->data[512], bp->blkno+1, 1);

	return 0;
}

int bflush(u32 dev) {
	int ret = 0;

	if(blist->valid && blist->dev == dev) {
		ret = bwrite(blist);
	}
	return ret;
}
