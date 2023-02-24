#include <virtio.h>
#include <types.h>
#include <string.h>
#include <printk.h>
#include <riscv.h>
#include <buf.h>
#include <panic.h>
#include <vm.h>
#include <devsw.h>
#include <fs.h>

struct inode inode[NINODE];
struct super_block sb[NSUPERBLK];

void fsinit(void) {
	struct buf *bp;

	bdevsw[rootdev.major].open();
	bp = bread(rootdev, SUPERBLOCK);
	memcpy(&sb, bp->data, sizeof(struct super_block));

	mount[0].dev = rootdev;
	// TODO:
	mount[0].sb = (char*)&sb;
	mount[0].ip = namei("/");
}

struct super_block *getfs(dev_t dev) {
	for(struct mount *p = &mount[0]; p < &mount[NMOUNT]; p++) {
		if(p->dev.major == dev.major && p->dev.minor == dev.minor) {
			return (struct super_block *)p->sb;
		}
	}
	panic("no fs\n");
	return NULL;
}

struct inode *iget(dev_t dev, u64 inum) {
	struct buf *buf;
	u64 offset;
	struct super_block *sb;

	sb = getfs(dev);
	inum--;
	offset = (SUPERBLOCK + sb->imap_blocks + sb->zmap_blocks + (inum / NINODE));
	buf = bread(dev, (offset*BLOCKSIZE)/SECTORSIZE);
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

	buf = bread(rootdev, zmap(ip, zone));
	dp = (struct direct *)buf->data;
	for(int i = ip->size; i; i -= sizeof(struct direct)) {
		if(strcmp(dp->name, name) == 0) {
			return iget(rootdev, dp->ino);
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
		ip = iget(rootdev, ROOT);
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
		buf = bread(rootdev, zmap(ip, zone));
		memcpy(dest, &buf->data[(offset % BLOCKSIZE)], size-total);
		total = total + (size - total);
	}

	while(total < size) {
		buf = bread(rootdev, zmap(ip, zone));
		memcpy(dest, buf->data, size - total);
		zone++;
		total = total + (size - total);
	}
	return total;
}

static int ffs(int x) {
	int bit;

	if(x == 0) {
	   return 0;
	}
	for (bit = 1; !(x & 1); bit++) {
	   x = (unsigned int)x >> 1;
	}
	return bit;
}

struct inode *ialloc(dev_t dev) {
	struct super_block *sb;
	struct inode *ip = NULL;
	u64 offset;
	u8 pos;
	struct buf *buf;

	sb = getfs(dev);
	offset = SUPERBLOCK + 1;
	buf = bread(dev, (offset*BLOCKSIZE)/SECTORSIZE);

	for(u64 i = 0; i < sb->imap_blocks; i+=8) {
		//TODO:
		if((pos = ffs(~buf->data[i] & 0xff)) != 0) {
			buf->data[i] = buf->data[i] | (1 << (pos-1));
			bwrite(buf);
			return iget(dev, 1<<(pos-1));
		}
	}
	return ip;
}

// TODO: writei

u8 bmapget(u64 bmap, u64 inum) {
	struct buf *buf;
	u64 offset;
	u8 bitmap;

	offset = bmap + (inum / (BLOCKSIZE * 8));
	buf = bread(rootdev, (offset*BLOCKSIZE)/SECTORSIZE);
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
