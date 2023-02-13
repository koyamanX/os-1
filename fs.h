#ifndef FS_H
#define FS_H

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

#endif
