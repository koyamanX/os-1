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
	virtio_req(blist->data, blist->blkno, 0);
	
	return blist;
}

int bwrite(struct buf *bp) {
	if(!bp->valid) {
		return -1;
	}
	virtio_req(bp->data, bp->blkno, 1);

	return 0;
}

int bflush(u32 dev) {
	int ret = 0;

	if(blist->valid && blist->dev == dev) {
		ret = bwrite(blist);
	}
	return ret;
}
