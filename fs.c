#include "fs.h"
#include "virtio.h"
#include "types.h"
#include "string.h"
#include "printk.h"
#include "buf.h"

struct super_block sb;
struct inode inode[16];

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
}

void fs_read_root(void) {
	;
}
