#include <fs.h>
#include <virtio.h>
#include <types.h>
#include <string.h>
#include <printk.h>
#include <riscv.h>
#include <buf.h>
#include <panic.h>
#include <vm.h>

struct super_block sb;
struct inode inode[NINODE];

void read_super(void) {
	struct buf *bp;
	bp = bread(VIRTIO_BLK, SUPERBLOCK);
	memcpy(&sb, bp->data, sizeof(struct super_block));

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

	if(sb.block_size != BLOCKSIZE) {
		panic("read_super: Unsupported block size\n");
	}
}

void fsinit(void) {
	read_super();
	struct inode *ip = namei("/usr/sbin/init");

	char *buf = kalloc();
	readi(ip, buf, 0, 1024);
}

struct inode *iget(u32 dev, u64 inum) {
	struct buf *buf;
	u64 offset;

	inum--;
	offset = (SUPERBLOCK + sb.imap_blocks + sb.zmap_blocks + (inum / NINODE));
	buf = bread(VIRTIO_BLK, (offset*BLOCKSIZE)/SECTORSIZE);
	memcpy(&inode[0], buf->data, sizeof(struct inode) * NINODE);
	
	return &inode[inum % NINODE];
}

char *dirname(char *path) {
	char *bp;

	bp = path;
	if(*bp == '/' && *(bp+1) == '\0') {
		// "/"
		return bp;
	}
	if(*bp == '.') {
		// "." or ".." -> "."
		*(bp+1) = '\0';
		return bp;
	}
	if(*bp != '/') {
		// reletive path assuming CWD -> "."
		*bp = '.';
		*(bp+1) = '\0';
		return bp;
	}
	bp++; // skip root "/"
	while(*bp) {
		if(*bp == '/' && *(bp+1) == '\0') {
			// remove last "/"
			*(bp+1) = '\0';
			return bp;
		}
		if(*bp == '/') {
			// remove last directory
			*bp = '\0';
			return path;
		}
		bp++;
	}
	return path;
}

char *basename(char *path) {
	return NULL;
}

struct inode *diri(struct inode *ip, char *name) {
	struct direct *dp;
	struct buf *buf;
	u8 zone = 0;

	buf = bread(VIRTIO_BLK, zmap(ip, zone));
	dp = (struct direct *)buf->data;
	for(int i = ip->size; i; i -= sizeof(struct direct)) {
		if(strcmp(dp->name, name) == 0) {
			return iget(VIRTIO_BLK, dp->ino);
		}
		dp++;
	}

	return NULL;
}

struct inode *namei(char *path) {
	struct inode *ip;
	char *name;
	struct inode *p;

	if(*path == '/') {
		ip = iget(VIRTIO_BLK, ROOT);
		path++;
	}
	
	name = strtok(path, "/");
	if(name) {
		p = diri(ip, name);
		if(p != NULL) {
			ip = p;
		}
	}
	while((name = strtok(NULL, "/")) != NULL) {
		p = diri(ip, name);
		if(p != NULL) {
			ip = p;
		}
	}

	return ip;
}

// read content pointed by ip to dest, for size bytes start from offset.
// TODO: implement offset.
u64 readi(struct inode *ip, char *dest, u64 offset, u64 size) {
	u64 total = 0;
	int zone = 0;
	struct buf *buf;

	if(offset > 0) {
		for(u64 i = BLOCKSIZE; i <= offset; i+=BLOCKSIZE) {
			zone++;
		}
		buf = bread(VIRTIO_BLK, zmap(ip, zone));
		memcpy(dest, &buf->data[(offset % BLOCKSIZE)], size-total);
		total = total + (size - total);
	}

	while(total < size) {
		buf = bread(VIRTIO_BLK, zmap(ip, zone));
		memcpy(dest, buf->data, size - total);
		zone++;
		total = total + (size - total);
	}
	return total;
}

u8 bmapget(u64 bmap, u64 inum) {
	struct buf *buf;
	u64 offset;
	u8 bitmap;

	offset = bmap + (inum / (BLOCKSIZE * 8));
	buf = bread(VIRTIO_BLK, (offset*BLOCKSIZE)/SECTORSIZE);
	bitmap = buf->data[inum % BLOCKSIZE];

	return bitmap;
}

u64 zmap(struct inode *ip, u64 zone) {
	// TODO: handle indirect zone
	u64 addr;
	
	if(zone >= 8) {
		panic("zmap: Indirect zone is not supported\n");
	}
	addr = (ip->zone[zone] * BLOCKSIZE) / SECTORSIZE;

	return addr;
}
