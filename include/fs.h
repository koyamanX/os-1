/**
 * @file fs.h
 * @brief Structures and function prototypes for low-level
 * 			access to minix3 filesystem.
 * @author ckoyama(koyamanX)
 */

#ifndef _FS_H
#define _FS_H

#include <devsw.h>
#include <riscv.h>

/**
 * @brief Struct for minix3 filesystem superblock.
 * @details Contains in-disk superblock of minix3 filesystem.
 */
struct super_block {
    u32 ninodes;          //!< Number of available inodes.
    u16 nzones;           //!< Number of available zones.
    u16 imap_blocks;      //!< Number of blocks used by inode bit map.
    u16 zmap_blocks;      //!< Number of blocks used by zone bit map.
    u16 first_data_zone;  //!< Number of first data zone.
    u16 log_zone_size;    //!< Log2 of block/zone.
    u16 pad;              //!< Padding.
    u32 max_size;         //!< Max file size.
    u32 zones;            //!< Number of zones
    u16 magic;            //!< Magic number
    u16 pad2;             //!< Padding
    u16 block_size;       //!< Block size in bytes
    u8 disk_version;      //!< Filesystem sub-version
};

#define FS_MAGIC 0x4d5a
#define FS_VERSION 3

#define NINODE 16      //!< Size of inode in one block
#define NICACHE 128    //!< Size of inode chache
#define INODE_SIZE 64  //!< Size of on disk inode

#define DIRECTZONE 6    //!< Direct zone number
#define INDIRECTZONE 7  //!< Indirect zone number

// TODO: 1024 == sb->block_size
#define NINDIRECTZONE \
    (1024 / sizeof(u32))  //!< Number of zones in each indirect zone.

#define IMAP 0  //!< IMAP number for alloc/free_bit
#define ZMAP 1  //!< ZMAP number for alloc/free_bit

/**
 * @brief Struct for minix3 inode.
 * @details First part of this struct is on-disk structure,
 * 		followed by on-memory structure.
 */
struct inode {
    u16 mode;      //!< File type, protection, etc. (On-disk)
    u16 nlinks;    //!< Number of links of this file. (On-disk)
    u16 uid;       //!< User id. (On-disk)
    u16 gid;       //!< Group id. (On-disk)
    u32 size;      //!< Size in bytes. (On-disk)
    u32 atime;     //!< Last access time. (On-disk)
    u32 mtime;     //!< Modification time of file contents. (On-disk)
    u32 ctime;     //!< Changed time of inode. (On-disk)
    u32 zone[10];  //!< Zones, first 8 are direct zone,
                   //!< next one is indirect zone, last one is triple indirect
                   //!< zone. (On-disk)
    dev_t dev;     //!< Device on this inode exist. (On-memory)
    ino_t inum;    //!< Inode number of this inode. (On-memory)
    u64 count;     //!< Reference count of this inode. (On-memory)
};

/**
 * Initialize minix3 filesystem.
 * @attention Call before using filesystem.
 */
void fsinit(void);

/**
 * Get superblock of device pointed by dev structure.
 * @param[in] dev device to obtain superblock.
 * @return Pointer to superblock or NULL.
 * @attention It allocate super block cache, \ref sb.
 */
struct super_block *getfs(dev_t dev);

/**
 * Get inode of inum on dev.
 * @param[in] dev device to obtain inode.
 * @param[in] inum inode nubmer.
 * @return inode or NULL.
 */
struct inode *iget(dev_t dev, u64 inum);

/**
 * Get inode pointed by path.
 * @param[in] path pathname to obtain inode of.
 * @return Pointer to inode associated with path or NULL.
 */
struct inode *namei(const char *path);

/**
 * Get inode of name in ip.
 * @param[in] ip pointer to inode to search name.
 * @param[in] name of file.
 * @return Pointer to inode on success or NULL.
 * @attention Indirect zone is not supported.
 */
struct inode *diri(struct inode *ip, char *name);

/**
 * Get block number of zone in ip.
 * @param[in] ip pointer to inode to get zone of.
 * @param[in] zone zone number.
 * @return block number of zone of ip.
 */
u64 zmap(struct inode *ip, u64 zone);

/**
 * Allocate new inode on dev.
 * @param[in] dev device to allocate inode on.
 * @return newly allocated inode or NULL on failure.
 */
struct inode *ialloc(dev_t dev);

/**
 * Update inode pointed by ip.
 * @param[in] ip inode to update.
 */
void iupdate(struct inode *ip);

/**
 * Put inode into disk if it is last reference and decrement reference counter.
 * If last reference, remove ip.
 * @param[in] ip Pointer to inode to put into disk.
 */
void iput(struct inode *ip);

/**
 *	Read content of inode zone from disk.
 *	@param[in] ip pointer to inode to obtain content of.
 *	@param[out] dest pointer to memory location to fill the content
 *obtained.
 *	@param[in] offset offset of inode zone to read.
 *	@param[in] size size in bytes to read.
 *	@return size read.
 */
u64 readi(struct inode *ip, char *dest, u64 offset, u64 size);

/**
 *	Write content of inode zone to disk.
 *	@param[in] ip pointer to inode to write content to.
 *	@param[out] dest pointer to memory location to read the content to
 *write.
 *	@param[in] offset offset of inode zone to write.
 *	@param[in] size size in bytes to write.
 *	@return size written.
 */
u64 writei(struct inode *ip, char *src, u64 offset, u64 size);

/**
 * @brief Find empty direct entry in dip and return its offset in dip.
 * @details Find empty direct entry in dip and return its offset in dip.
 * @param[in] dip Pointer to directory inode to search.
 * @return offset of free slot in dip.
 */
u64 newdirect(struct inode *dip);

#define ROOT 1              //!< Inode number of root directory.
#define SUPERBLOCK_BLKNO 1  //!< Block number of super block.
#define IMAP_BLKNO(sb) (2)  //!< Block number of first inode map.
#define ZMAP_BLKNO(sb) \
    (IMAP_BLKNO(sb) + sb->imap_blocks)  //!< Block number of first zone map.

#define UNUSED_ZONE 0

#define I_TYPE 0170000           //!< Bit mask for type of file.
#define I_REGULAR 0100000        //!< Inode type of regular file.
#define I_BLOCK_SPECIAL 0060000  //!< Inode type of block special device.
#define I_DIRECTORY 0040000      //!< Inode type of direcotry.
#define I_CHAR_SPECIAL 0020000   //!< Inode type of character special device.
#define I_NAMED_PIPE 0010000     //!< Inode type of named pipe.
#define I_SET_UID_BIT 0004000    //!< Set-uid bit.
#define I_SET_GID_BIT 0002000    //!< Set-gid bit.
#define ALL_MODES 0006777        //!< All modes.
#define RWX_MODES 0000777        //!< Read/Write/Execute for all.
#define R_BIT 0000004            //!< Read bit.
#define W_BIT 0000002            //!< Write bit.
#define X_BIT 0000001            //!< Execute bit.
#define I_FREE 0000000           //!< Free.

#define DIRSIZ 60  //!< Size of directory name.

/**
 * @brief Struct for directory entry.
 * @details On-disk structure of directory entry.
 */
struct direct {
    u32 ino;            //!< Inode number this direct entry associated with.
    char name[DIRSIZ];  //!< Name of file.
};

#define NSUPERBLK 1              //!< Size of on-memory superblock cache.
extern struct super_block sb[];  //!< Superblock cache, only superblock of minix
                                 //!< 3 filesystem is supported.
extern struct inode inode[NICACHE];  //!< On-memory inode cache.

char *kstrdup(const char *str);

#endif /* _FS_H */
