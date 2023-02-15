#include "fs.h"
#include "virtio.h"
#include "types.h"
#include "string.h"
#include "printk.h"
#include "riscv.h"
#include "buf.h"
#include "panic.h"

struct super_block sb;
struct inode inode[NINODE];

void read_super(void) {
	struct buf *bp;
	bp = bread(0, 2);
	memcpy(&sb, bp->data, sizeof(struct super_block));

	printk("\n\n\n");

	printk("ninodes: %x\n", sb.ninodes);
	printk("nzones: %x\n", sb.nzones);
	printk("imap_blocks: %x\n", sb.imap_blocks);
	printk("zmap_blocks: %x\n", sb.zmap_blocks);
	printk("firstdatazone: %x\n", sb.firstdatazone);
	printk("log_zone_size: %x\n", sb.log_zone_size);
	printk("max_size: %x\n", sb.max_size);
	printk("zones: %x\n", sb.zones);
	printk("magic: %x\n", sb.magic);
	printk("block_size: %x\n", sb.block_size);
	printk("disk_version: %x\n", sb.disk_version);

#if 0
	memset(buf, 0, 512);
	u64 offset = 2 + sb.imap_blocks + sb.zmap_blocks;
	virtio_req(buf, (offset*1024)/512, 0);
	printk("block address of inode: %x, offset: %x\n", (offset*1024)/512, offset);
	memcpy(&inode[0], buf, 512);
	for(int i = 0; i < 16; i++) {
		printk("inode[%x].mode: %x\n", i, inode[i].mode);
		printk("isdir?: %x\n", !!(inode[i].mode & S_IFDIR));
		for(int j = 0; j < 10; j++) {
			printk("inode[%x].zone[%x]: %x\n", i, j, inode[i].zone[j]);
		}
	}

	memset(buf, 0, 512);
	virtio_req(buf, (inode[0].zone[0]*1024)/512, 0);
	struct direct dir[16];
	memcpy(&dir, buf, 512);
	for(int i = 0; i < 8; i++) {
		printk("dir.name: %s\n", dir[i].name);
		printk("dir.inode: %x\n", dir[i].ino);
	}

	memset(buf, 0, 512);
	virtio_req(buf, (inode[1].zone[0]*1024)/512, 0);
	printk("%s\n", buf);

	printk("\n\n\n");
#endif
}

void fsinit(void) {
	read_super();
	struct inode *rooti = namei("/");
	printk("fsinit: rooti->mode: %x\n", rooti->mode);
	printk("fsinit: rooti->size: %x\n", rooti->size);
}

struct inode *iget(u32 dev, u64 inum) {
	struct buf *buf;
	u64 offset;
	u8 bitmap;

	inum--;
	offset = (2 + sb.imap_blocks + sb.zmap_blocks + (inum / NINODE));
	buf = bread(0, (offset*1024)/512);
	memcpy(&inode[0], buf->data, sizeof(struct inode) * NINODE);
	
	bitmap = bmapget(IMAP(sb), inum);
	bitmap = bitmap >> inum % 8;

	return bitmap ? &inode[inum % NINODE] : NULL;
}

struct inode *diri(struct inode *ip, char *name) {
	return NULL;
}

struct inode *namei(char *path) {
	struct inode *ip;

	if(*path == '/') {
		ip = iget(0, 1);
	}
	return ip;
}

u8 bmapget(u64 bmap, u64 inum) {
	struct buf *buf;
	u64 offset;
	u8 bitmap;

	offset = bmap + (inum / (1024 * 8));
	buf = bread(0, (offset*1024)/512);
	bitmap = buf->data[inum % 1024];

	return bitmap;
}

u64 zmap(struct inode *ip, u64 zone) {
	// TODO: handle indirect zone
	u64 addr;
	
	if(zone >= 8) {
		panic("zmap: Indirect zone is not supported\n");
	}
	addr = (ip->zone[zone] * 1024);

	return addr;
}
