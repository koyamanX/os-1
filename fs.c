#include "fs.h"
#include "virtio.h"
#include "types.h"
#include "string.h"
#include "printk.h"

char buf[512];
struct super_block sb;

void fs_dump_super_block(void) {
	memset(buf, 0, 512);
	virtio_req(buf, 2, 0);
	memcpy(&sb, buf, sizeof(struct super_block));

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
}
