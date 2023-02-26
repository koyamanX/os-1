#ifndef FS_H
#define FS_H

#include <sys/types.h>
#include <devsw.h>

struct super_block {
	u32 ninodes;
	u16 nzones;
	u16 imap_blocks;
	u16 zmap_blocks;
	u16 firstdatazone;
	u16 log_zone_size;
	u16 pad;
	u32 max_size;
	u32 zones;
	u16 magic;
	u16 pad2;
	u16 block_size;
	u8 disk_version;
};

#define NINODE 16
struct inode {
	// on disk structure
	u16 mode;
	u16 nlinks;
	u16 uid;
	u16 gid;
	u32 size;
	u32 atime;
	u32 mtime;
	u32 ctime;
	u32 zone[10];
	// on memory structure
	dev_t dev;
	ino_t inum;
	u64 count;
};
#define INODE_SIZE 64

struct super_block *getfs(dev_t dev);
void fsinit(void);
struct inode *iget(dev_t dev, u64 inum);
struct inode *namei(char *path);
struct inode *diri(struct inode *ip, char *name);
u8 bmapget(u64 bmap, u64 inum);
u64 zmap(struct inode *ip, u64 zone);
char *dirname(char *path);
char *basename(char *path);
struct inode *ialloc(dev_t dev);
void iupdate(struct inode *ip);
void iput(struct inode *ip);

u64 readi(struct inode *ip, char *dest, u64 offset, u64 size);

extern struct inode inode[NINODE];

/* 
readi();
writei();
openi();
closei();
bmap();
namei();
*/

#define IMAP(sb) (2)
#define ZMAP(sb) (2+sb->imap_blocks)

#define S_IFDIR 0x6000

#define I_TYPE          0170000
#define I_REGULAR       0100000
#define I_BLOCK_SPECIAL 0060000

#define I_DIRECTORY     0040000
#define I_CHAR_SPECIAL  0020000
#define I_NAMED_PIPE    0010000
#define I_SET_UID_BIT   0004000
#define I_SET_GID_BIT   0002000
#define ALL_MODES       0006777
#define RWX_MODES       0000777
#define R_BIT           0000004
#define W_BIT           0000002
#define X_BIT           0000001
#define I_NOT_ALLOC     0000000

#define DIRSIZ 60

#define SUPERBLOCK 2
#define ROOT 1

struct direct {
	u32 ino;
	char name[DIRSIZ];
};

#define NSUPERBLK 10
extern struct super_block sb[];
#endif
