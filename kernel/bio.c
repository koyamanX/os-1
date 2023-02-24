#include <buf.h>
#include <vm.h>
#include <virtio.h>
#include <devsw.h>

struct buf *blist;

void binit(void) {
	blist = (struct buf *)kalloc();	
	blist->valid = 0;
}

struct buf *bread(dev_t dev, u64 blkno) {
	if(blist->valid && blist->blkno == blkno) {
		return blist;
	}
	if(blist->valid) {
		bwrite(blist);
	}
	blist->valid = 1;
	blist->dev = dev;
	blist->blkno = blkno;
	bdevsw[dev.major].strategy(blist->data, blist->blkno, 0);
	
	return blist;
}

int bwrite(struct buf *bp) {
	if(!bp->valid) {
		return -1;
	}
	bdevsw[bp->dev.major].strategy(bp->data, bp->blkno, 1);
	bp->valid = 0;

	return 0;
}

int bflush(dev_t dev) {
	int ret = 0;

	if(blist->valid && blist->dev.major == dev.major && blist->dev.minor == dev.minor) {
		ret = bwrite(blist);
	}
	return ret;
}
