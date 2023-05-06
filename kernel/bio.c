#include <buf.h>
#include <devsw.h>
#include <sys/types.h>
#include <virtio.h>
#include <vm.h>

static struct buf buf[NBUF];
static struct buf *bfreelist;

void binit(void) {
    // Initialize all buffer entries
    for (int i = 0; i < NBUF; i++) {
        buf[i].valid = 0;
        buf[i].dirty = 0;
        buf[i].next = (i == NBUF - 1) ? NULL : &buf[i + 1];
    }

    // Set bfreelist to the first buffer entry
    bfreelist = &buf[0];
}

struct buf *bread(dev_t dev, u64 blkno) {
    struct buf *bp = getblk(dev, blkno);
    // Read the block into the buffer from the device
    if (bdevsw[major(dev)].strategy != NULL) {
        (*bdevsw[major(dev)].strategy)(bp->data, bp->blkno, 0);
    }
    bp->valid = 1;

    return bp;
}

void bwrite(struct buf *bp) {
    if (bp == NULL) {
        return;
    }

    if (!bp->valid) {
        return;
    }

    struct bdevsw *dev = &bdevsw[major(bp->dev)];

    // Call device specific write function
    if (dev->strategy != NULL) {
        dev->strategy(bp->data, bp->blkno, 1);
    }

    // Mark buffer as clean
    bp->dirty = 0;
}

void bflush(dev_t dev) {
    struct buf *bp;
    struct bdevsw *bdev = &bdevsw[major(dev)];

    // Flush all bpfers associated with dev from the active list
    for (bp = bdev->bactivelist; bp; bp = bp->next) {
        if (bp->dev == dev && bp->dirty && bp->valid) {
            bwrite(bp);
        }
    }
}

struct buf *getblk(dev_t dev, u64 blkno) {
    struct buf *bp;

    for (bp = bfreelist; bp != NULL; bp = bp->next) {
        if (bp->valid == 0) {
            if (bp == bfreelist) {
                bfreelist = bp->next;
            } else {
                struct buf *prev = bfreelist;
                while (prev->next != bp) {
                    prev = prev->next;
                }
                prev->next = bp->next;
            }
            break;
        }
    }

    if (bp == NULL) {
        struct bdevsw *bdev = &bdevsw[major(dev)];
        bp = bdev->bactivelist;
        if (bp == NULL) {
            return NULL;
        }
        bwrite(bp);
    }

    bp->valid = 1;
    bp->dev = dev;
    bp->blkno = blkno;
    bp->dirty = 0;

    return bp;
}

void brelse(struct buf *bp) {
    if (!bp) {
        // Do nothing if the buffer pointer is null.
        return;
    }

    if (bp->dirty && bp->valid) {
        // If the buffer is dirty and valid, write it out and mark it as not
        // dirty.
        bwrite(bp);     // Write the contents of the buffer to disk.
        bp->dirty = 0;  // Clear the dirty flag to indicate that the buffer is
                        // no longer dirty.
    }

    bp->valid = 0;
    bp->next = bfreelist;  // Set the next pointer of the buffer to the current
                           // head of the freelist.
    bfreelist = bp;  // Update the head of the freelist to point to the buffer.
}
