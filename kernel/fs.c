/**
 * @file fs.c
 * @brief Implementation of minix 3 filesystem.
 * @author ckoyama(koyamanX)
 */

#include <buf.h>
#include <devsw.h>
#include <fcntl.h>
#include <file.h>
#include <fs.h>
#include <libgen.h>
#include <panic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <slob.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <virtio.h>
#include <vm.h>

struct inode inode[NICACHE];
struct super_block sb[NSUPERBLK];

static int ffs(int x);
static u64 alloc_bit(dev_t dev, int map);
static void free_bit(dev_t dev, int map, u64 pos);
static u8 has_bit(dev_t dev, int map, u64 n);
static char *kstrdup(const char *str);

void fsinit(void) {
    struct buf *bp;
    char rootdir[] = "/";

    if (bdevsw[rootdev.major].open) {
        // Call device dependent open if exist.
        bdevsw[rootdev.major].open();
    }
    // Read Superblock.
    bp = bread(rootdev, SUPERBLOCK_BLKNO);
    // Copy superblock to superblock cache.
    memcpy(&sb, bp->data, sizeof(struct super_block));
    // Zero clear file cache.
    memset(&file, 0, sizeof(struct file) * NFILE);
    // Zero clear inode cache.
    memset(&inode, 0, sizeof(struct inode) * NICACHE);
    // Zero clear mount cache.
    memset(&mount, 0, sizeof(struct mount) * NMOUNT);
    brelse(bp);

    // Create mount point for root directory.
    mount[0].dev = rootdev;
    mount[0].sb = sb;
    mount[0].ip = namei(rootdir);
}

struct super_block *getfs(dev_t dev) {
    for (struct mount *p = &mount[0]; p < &mount[NMOUNT]; p++) {
        if (p == NULL) {
            continue;
        }
        if (p->dev.major == dev.major && p->dev.minor == dev.minor) {
            // If matching device found on mount point, return superblock.
            return p->sb;
        }
    }
    panic("No fs\n");

    return NULL;
}

static char *kstrdup(const char *str) {
    char *p = NULL;

    p = kmalloc(strlen(str) + 1);
    strcpy(p, str);

    return p;
}

struct inode *iget(dev_t dev, u64 inum) {
    u64 blkno;
    struct super_block *sb;
    struct inode *ip;

    for (int i = 0; i < NICACHE; i++) {
        // Find unused inode cache entry.
        if (inode[i].count == 0) {
            sb = getfs(dev);
            /**
             * Inode number starts from 1, however block number starts with 0,
             * so subtract by 1 to map block number.
             */
            inum--;
            blkno = (IMAP_BLKNO(sb) + sb->imap_blocks + sb->zmap_blocks +
                     (inum / NINODE));
            if (!has_bit(dev, 0, inum)) {
                struct buf *buf;
                buf = bread(dev, blkno);
                // Read inode to inode cache.
                memcpy(&inode[i], (buf->data + ((inum % NINODE) * INODE_SIZE)),
                       INODE_SIZE);
                brelse(buf);
            }
            // Create on-memory inode entry.
            ip = &inode[i];
            ip->count++;
            ip->dev = dev;
            ip->inum = inum;

            return ip;
        }
    }
    panic("No empty inode cache entry\n");

    return NULL;
}

struct inode *diri(struct inode *ip, char *name) {
    struct direct *dp;
    struct buf *buf;
    struct super_block *sb;
    int size;

    sb = getfs(ip->dev);
    size = ip->size;
    for (u8 zone = 0; zone < DIRECTZONE; zone++) {
        buf = bread(rootdev, zmap(ip, zone));
        dp = (struct direct *)buf->data;
        for (int i = 0; i < sb->block_size; i += sizeof(struct direct)) {
            if (size == 0) {
                return NULL;
            }
            if (strcmp(dp->name, name) == 0) {
                brelse(buf);
                return iget(rootdev, dp->ino);
            }
            dp++;
            size -= sizeof(struct direct);
        }
        brelse(buf);
    }
    // TODO: Indirect zone

    return NULL;
}

struct inode *namei(const char *path) {
    char *p;
    char *name;
    struct inode *dp;
    struct inode *ip;

    p = kstrdup(path);
    dp = iget(rootdev, ROOT);

    if (*path != '/') {
        panic("Relative path is not supported yet\n");
    }

    name = strtok(p, "/");
    if (name == NULL || !*name) {
        kfree(p);
        return dp;
    }
    do {
        ip = diri(dp, name);
        name = strtok(NULL, "/");
        dp = ip;
    } while (name);
    kfree(p);

    return ip;
}

u64 readi(struct inode *ip, char *dest, u64 offset, u64 size) {
    u64 total = 0;
    int zone = 0;
    struct buf *buf;
    size_t sz;
    size_t off;

    if ((ip->mode & S_IFMT) == S_IFCHR) {
        VERBOSE_PRINTK("readi: S_IFCHR\n");
        for (u64 i = 0; i < size; i++) {
            // If character device read one by one.
            dest[i] = cdevsw[ip->dev.major].read();
        }
        return size;
    }

    if (offset >= ip->size) {
        // EOF
        return 0;
    }

    if (offset > 0) {
        // TODO: Indirect zone.
        // TODO: Allocate zone if not used.
        for (u64 i = BLOCKSIZE; i <= offset; i += BLOCKSIZE) {
            // Find zone which is ranged of an offset.
            zone++;
        }
        // Read the zone.
        buf = bread(rootdev, zmap(ip, zone));
        // Offset in block is offset % BLOCKSIZE.
        off = offset % BLOCKSIZE;
        sz = size % BLOCKSIZE;
        // Write data to dest for sz bytes.
        memcpy(dest, &buf->data[off], sz);
        brelse(buf);
        total += sz;
        size -= sz;
        // Advance dest pointer by size for next write.
        dest = &dest[sz];
    }

    for (int i = size / BLOCKSIZE; i; i--) {
        sz = BLOCKSIZE;
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(dest, buf->data, sz);
        brelse(buf);
        // Update corresponding inode's size.
        total += sz;
        size -= sz;
        // Advance dest pointer by size for next write.
        dest = &dest[sz];
        // Size of each read for non-last zone is BLOCKSIZE,
        // So advance zone by one.
        // Only last read is non-BLOCKSIZE read.
        zone++;
    }

    if (size) {
        // TODO: Indirect zone.
        // TODO: Allocate zone if not used.
        buf = bread(rootdev, zmap(ip, zone));
        // If we can read BLOCKSIZE.
        // Wrap around BLOCKSIZE, since access unit is BLOCKSIZE.
        // In this case, it is last block to read.
        sz = size % BLOCKSIZE;
        // Write data to dest for sz bytes.
        // Offset is handled before, so we can ignore offset.
        memcpy(dest, buf->data, sz);
        brelse(buf);
        // Update corresponding inode's size.
        total += sz;
        size -= sz;
        // Advance dest pointer by size for next write.
        dest = &dest[sz];
        // Size of each read for non-last zone is BLOCKSIZE,
        // So advance zone by one.
        // Only last read is non-BLOCKSIZE read.
        zone++;
    }

    return total;
}

u64 writei(struct inode *ip, char *src, u64 offset, u64 size) {
    u64 total = 0;
    int zone = 0;
    struct buf *buf;
    size_t sz;
    size_t off;

    if ((ip->mode & S_IFMT) == S_IFCHR) {
        VERBOSE_PRINTK("writei: S_IFCHR\n");
        // There is no offset in Character device.
        for (u64 i = 0; i < size; i++) {
            // Character device accept data to be written one byte one.
            cdevsw[ip->dev.major].write(src[i]);
        }
        return size;
    }

    if (offset > 0) {
        // TODO: Indirect zone.
        // TODO: Allocate zone if not used.
        for (u64 i = BLOCKSIZE; i <= offset; i += BLOCKSIZE) {
            // Find zone which is ranged of an offset.
            if (zmap(ip, zone) == UNUSED_ZONE) {
                ip->zone[zone] = alloc_bit(ip->dev, ZMAP);
            }
            zone++;
        }
        // Read the zone.
        buf = bread(rootdev, zmap(ip, zone));
        // Offset in block is offset % BLOCKSIZE.
        off = offset % BLOCKSIZE;
        sz = size % BLOCKSIZE;
        // Write data from src for sz bytes.
        memcpy(&buf->data[off], src, sz);
        bwrite(buf);
        brelse(buf);
        ip->size += sz;
        total += sz;
        size -= sz;
    }

    for (int i = size / BLOCKSIZE; i; i--) {
        if (zmap(ip, zone) == UNUSED_ZONE) {
            ip->zone[zone] = alloc_bit(ip->dev, ZMAP);
        }
        // TODO: Indirect zone.
        // TODO: Allocate zone if not used.
        buf = bread(rootdev, zmap(ip, zone));
        // If we can write BLOCKSIZE.
        sz = BLOCKSIZE;
        // Write data from src for sz bytes.
        // Offset is handled before, so we can ignore offset.
        memcpy(buf->data, src, sz);
        bwrite(buf);
        brelse(buf);
        // Update corresponding inode's size.
        ip->size += sz;
        total += sz;
        size -= sz;
        // Size of each write for non-last zone is BLOCKSIZE,
        // So advance zone by one.
        // Only last write is non-BLOCKSIZE write.
        zone++;
    }

    if (size) {
        if (zmap(ip, zone) == UNUSED_ZONE) {
            ip->zone[zone] = alloc_bit(ip->dev, ZMAP);
        }
        // TODO: Indirect zone.
        // TODO: Allocate zone if not used.
        buf = bread(rootdev, zmap(ip, zone));
        // Wrap around BLOCKSIZE, since access unit is BLOCKSIZE.
        // In this case, it is last block to write.
        sz = size % BLOCKSIZE;
        // Write data from src for sz bytes.
        // Offset is handled before, so we can ignore offset.
        memcpy(buf->data, src, sz);
        bwrite(buf);
        brelse(buf);
        // Update corresponding inode's size.
        ip->size += sz;
        total += sz;
        size -= sz;
        // Size of each write for non-last zone is BLOCKSIZE,
        // So advance zone by one.
        // Only last write is non-BLOCKSIZE write.
        zone++;
    }
    // Write out inode.
    iupdate(ip);

    // Return total bytes written.
    return total;
}

/**
 *	@brief Find first set bit.
 *	@param[in] bit to search.
 *	@return position of first set bit.
 */
static int ffs(int x) {
    int bit;

    if (x == 0) {
        return 0;
    }
    for (bit = 1; !(x & 1); bit++) {
        x = (unsigned int)x >> 1;
    }
    return bit;
}

/**
 * @brief Allocate new entry from map.
 * @details Allocate new entry from map.
 * @param[in] dev device to allocate bit map.
 * @param[in] map IMAP for inode map, ZMAP for zone map.
 * @return number of zone or inode entry.
 */
static u64 alloc_bit(dev_t dev, int map) {
    struct super_block *sb;
    struct buf *buf;
    u64 inum;
    u8 pos;
    u64 last_block;
    u64 map_offset;

    sb = getfs(dev);
    if (map == IMAP) {
        last_block = sb->imap_blocks;
        map_offset = IMAP_BLKNO(sb);
    } else if (map == ZMAP) {
        last_block = sb->zmap_blocks;
        map_offset = ZMAP_BLKNO(sb);
    } else {
        panic("Unknown map");
    }
    for (u64 blkoff = 0; blkoff < last_block; blkoff++) {
        buf = bread(dev, map_offset + blkoff);
        for (u64 byteoff = 0; byteoff < sb->block_size; byteoff++) {
            u8 byte = buf->data[byteoff];
            if ((~byte != 0) && ((pos = ffs(~byte & 0xff)) != 0)) {
                byte |= (1 << (pos - 1));
                buf->data[byteoff] = byte;
                bwrite(buf);
                brelse(buf);
                inum = (blkoff * sb->block_size) + (byteoff * 8) + (pos - 1);

                return inum;
            }
        }
        brelse(buf);
    }
    panic("No free slots in bitmap\n");

    return -1;
}

/**
 * @brief Free entry to map.
 * @details Free entry to map.
 * @param[in] dev device to free bit map.
 * @param[in] map IMAP for inode map, ZMAP for zone map.
 * @param[in] pos number of zone or inode entry to free.
 */
__attribute__((unused)) static void free_bit(dev_t dev, int map, u64 pos) {
    struct super_block *sb;
    struct buf *buf;
    u64 blkoff;
    u64 byteoff;
    u64 bitoff;
    u64 map_offset;

    sb = getfs(dev);
    if (map == IMAP) {
        // IMAP
        map_offset = IMAP_BLKNO(sb);
        pos--;
    } else if (map == ZMAP) {
        // ZMAP
        map_offset = ZMAP_BLKNO(sb);
    } else {
        panic("Unknown map");
    }
    byteoff = (pos % (sb->block_size * 8));
    blkoff = pos / sb->block_size;
    bitoff = pos % 8;
    buf = bread(dev, map_offset + blkoff);
    u8 byte = buf->data[byteoff];
    byte &= ~(1 << bitoff);
    buf->data[byteoff] = byte;
    bwrite(buf);
    brelse(buf);
}

/**
 * @brief Find n in map is used or not.
 * @details Find n in map is used or not.
 * @param[in] dev device to free bit map.
 * @param[in] map IMAP for inode map, ZMAP for zone map.
 * @param[in] n number of zone or inode entry to search.
 * @return true if used, false if unused.
 * @attention ZMAP is not supported yet.
 */
static u8 has_bit(dev_t dev, int map, u64 n) {
    u64 blkoff;
    u64 byteoff;
    u8 bitoff;
    u8 byte;
    u8 bit;
    struct super_block *sb;
    struct buf *buf;

    sb = getfs(dev);
    if (map == ZMAP) {
        panic("Not supported yet\n");
    }

    blkoff = n / (sb->block_size * 8);
    byteoff = (n / 8) % sb->block_size;
    bitoff = n % 8;

    buf = bread(dev, blkoff);
    byte = buf->data[byteoff];
    bit = byte & (1 << bitoff);
    brelse(buf);

    return bit;
}

struct inode *ialloc(dev_t dev) {
    struct inode *ip = NULL;
    u64 inum;

    inum = alloc_bit(dev, IMAP);
    ip = iget(dev, inum);

    return ip;
}

void iupdate(struct inode *ip) {
    dev_t dev;
    struct super_block *sb;
    struct buf *buf;
    u64 offset;

    dev = ip->dev;
    sb = getfs(dev);
    offset = (IMAP_BLKNO(sb) + sb->imap_blocks + sb->zmap_blocks +
              (ip->inum / NINODE));
    buf = bread(dev, offset);
    memcpy(&buf->data[(ip->inum % NINODE) * INODE_SIZE], ip, INODE_SIZE);
    bwrite(buf);
    brelse(buf);
}

void iput(struct inode *ip) {
    if (ip->count == 1) {
        iupdate(ip);
    }
    // TODO: last reference
    ip->count--;
}

u64 zmap(struct inode *ip, u64 zone) {
    // TODO: handle indirect zone
    u64 addr;

    if (zone >= DIRECTZONE) {
        panic("zmap: Indirect zone is not supported\n");
    }
    addr = ip->zone[zone];

    return addr;
}

int open(const char *pathname, int flags, mode_t mode) {
    int fd = -1;
    struct inode *ip;
    struct inode *dip;
    char *basedir;
    char *filename;
    char *path;
    char *basec;
    char *dirc;
    u64 offset = 0;
    struct direct dir;
    struct file *fp;

    path = kstrdup(pathname);
    // Characters for basedir, since basedir may destroy original string.
    basec = kstrdup(pathname);
    // Characters for dirname, since dirname may destroy original string.
    dirc = kstrdup(pathname);

    if (strncmp(path, ".", 2) == 0) {
        // TODO: CWD is not supported for now
        basedir = "/";
    } else {
        // CAUTION: dirc may be destroyed after this call.
        basedir = dirname(dirc);
    }
    // CAUTION: basec may be destroyed after this call.
    filename = basename(basec);

    ip = namei(path);
    if (ip == NULL) {
        // If not exsit.
        if (!(flags & O_CREAT)) {
            // If not exists and O_CREAT is not set.
            goto free_and_exit;
        }
        // Get inode of basedir.
        dip = namei(basedir);
        if (dip == NULL) {
            // If basedir is not exists,
            // TODO: currect behaviour is creating direcotry structure first.
            goto free_and_exit;
        }
        // Create new direct entry in direcotry.
        dip->size += sizeof(struct direct);
        iupdate(dip);
        // Read until free slot found.
        do {
            readi(dip, (char *)&dir, offset, sizeof(struct direct));
            offset += sizeof(struct direct);
            VERBOSE_PRINTK("lookup: %s:%x\n", dir.name, dir.ino);
        } while (!(strcmp(dir.name, "") == 0) && dir.ino != 0);
        offset -= sizeof(struct direct);
        // Allocate new inode.
        ip = ialloc(dip->dev);
        // Set mode and size.
        ip->mode = mode | I_REGULAR;
        // Always trunc for newly created file.
        // Zone is expanded and allocated in write system call.
        ip->size = 0x0;

        // Update newly create inode.
        iupdate(ip);
        // Set newly created filename and its ino to empty direct entry.
        strcpy(dir.name, filename);
        // Inode number starts from 1.
        dir.ino = ip->inum + 1;
        // Write direct entry.
        writei(dip, (char *)&dir, offset, sizeof(struct direct));
    }
    // Allocate file descriptor.
    fd = ufalloc();
    // Allocate file structure.
    fp = falloc();
    fp->flags = mode;
    fp->ip = ip;
    fp->ip->mode |= mode;
    fp->ip->nlinks++;
    // TODO: uid, gid is constant 0.
    fp->ip->uid = 0;
    fp->ip->gid = 0;
    // TODO: atime, mtime and ctime is constant 0.
    fp->ip->atime = 0;
    fp->ip->mtime = 0;
    fp->ip->ctime = 0;
    // Trunc file.
    if (flags & O_TRUNC) {
        fp->ip->size = 0;
    }
    // Update inode.
    iupdate(fp->ip);

free_and_exit:
    kfree(path);
    kfree(basec);
    kfree(dirc);
    return fd;
}

int creat(const char *pathname, mode_t mode) {
    return open(pathname, (O_WRONLY | O_CREAT | O_TRUNC), mode);
}
int mkdir(const char *pathname, mode_t mode) {
    return open(pathname, (O_CREAT), (mode & RWX_MODES) | I_DIRECTORY);
}
int mknod(const char *pathname, mode_t mode, dev_t dev) {
    struct inode *ip;
    struct direct dir;
    struct file *fp;
    int fd = -1;
    char buf[128];   // TODO:
    char buf1[128];  // TODO:
    char *basedir;
    char *filename;

    // dirname, basename destroy 'str' argument.
    strcpy(buf, pathname);
    strcpy(buf1, pathname);
    basedir = dirname(buf);
    filename = basename(buf1);
    if (strncmp(basedir, ".", 2) == 0) {
        // TODO: CWD is not supported for now
        basedir = "/";
    }
    VERBOSE_PRINTK("basedir: %s, filename: %s\n", basedir, filename);
    ip = namei(basedir);
    if (ip == NULL) {
        return -1;
    }
    ip->size += sizeof(struct direct);
    iupdate(ip);
    u64 offset = 0;
    do {
        readi(ip, (char *)&dir, offset, sizeof(struct direct));
        offset += sizeof(struct direct);
        VERBOSE_PRINTK("lookup: %s:%x\n", dir.name, dir.ino);
    } while (!(strcmp(dir.name, "") == 0) && dir.ino != 0);
    fd = ufalloc();
    fp = falloc();
    fp->flags = mode;
    fp->ip = ialloc(ip->dev);
    fp->ip->mode = mode & RWX_MODES;
    if (S_ISBLK(mode)) {
        fp->ip->mode |= I_BLOCK_SPECIAL;
    } else if (S_ISCHR(mode)) {
        VERBOSE_PRINTK("mknod: I_CHAR_SPECIAL\n");
        fp->ip->mode |= I_CHAR_SPECIAL;
    }
    openi(fp->ip);
    fp->ip->nlinks++;
    fp->ip->size = 0x0;
    fp->ip->dev = dev;
    iupdate(fp->ip);
    strcpy(dir.name, filename);
    dir.ino = fp->ip->inum + 1;
    VERBOSE_PRINTK("%s:%x\n", dir.name, dir.ino);
    VERBOSE_PRINTK("%x\n", offset);
    offset -= sizeof(struct direct);
    writei(ip, (char *)&dir, offset, sizeof(struct direct));

    return fd;
}

int close(int fd) {
    struct proc *rp;
    struct inode *ip;
    struct file *fp;

    rp = cpus[r_tp()].rp;
    fp = rp->ofile[fd];
    ip = fp->ip;
    if (fp == NULL || ip == NULL) {
        return -1;
    }
    if (fp->count == 0) {
        panic("Closing unused file\n");
    }
    closei(ip);
    rp->ofile[fd] = NULL;

    return 0;
}
