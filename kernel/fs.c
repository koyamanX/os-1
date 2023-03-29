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

void fsinit(void) {
    struct buf *bp;
    char rootdir[] = "/";

    if (bdevsw[rootdev.major].open) {
        // Call device dependent open if exist.
        bdevsw[rootdev.major].open();
    }
    // Read Superblock.
    bp = bread(rootdev, SUPERBLOCK);
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
            blkno = (IMAP(sb) + sb->imap_blocks + sb->zmap_blocks +
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

struct inode *namei(char *path) {
    char *save;
    char *name;
    struct inode *dirp;
    struct inode *ip;

    save = kmalloc(strlen(path) + 1);
    strcpy(save, path);

    dirp = iget(rootdev, ROOT);

    if (*path != '/') {
        panic("Relative path is not supported yet\n");
    }

    name = strtok(path, "/");
    if (name == NULL || !*name) {
        kfree(save);
        return dirp;
    }
    do {
        ip = diri(dirp, name);
        name = strtok(NULL, "/");
        dirp = ip;
    } while (name);
    kfree(save);

    return ip;

#if 0
    struct inode *ip;
    char *name;
    struct inode *p;
    VERBOSE_PRINTK("namei: START\n");
    if (*path == '/') {
        VERBOSE_PRINTK("namei: %s\n", path);
        ip = iget(rootdev, ROOT);
        path++;
    }

    name = strtok(path, "/");
    if (name) {
        VERBOSE_PRINTK("namei: %s\n", name);
        p = diri(ip, name);
        if (p != NULL) {
            ip = p;
        }
    }
    while ((name = strtok(NULL, "/")) != NULL) {
        VERBOSE_PRINTK("namei: %s\n", name);
        p = diri(ip, name);
        if (p != NULL) {
            ip = p;
        }
    }

    VERBOSE_PRINTK("namei: END\n");

    return ip;
#endif
}

// read content pointed by ip to dest, for size bytes start from offset.
// TODO: implement offset.
u64 readi(struct inode *ip, char *dest, u64 offset, u64 size) {
    u64 total = 0;
    int zone = 0;
    struct buf *buf;

    if ((ip->mode & S_IFMT) == S_IFCHR) {
        VERBOSE_PRINTK("readi: S_IFCHR\n");
        for (u64 i = 0; i < size; i++) {
            dest[i] = cdevsw[ip->dev.major].read();
        }
        return size;
    }

    if (offset == ip->size) {
        // EOF
        return 0;
    }

    if (offset > 0) {
        for (u64 i = BLOCKSIZE; i <= offset; i += BLOCKSIZE) {
            zone++;
        }
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(dest, &buf->data[(offset % BLOCKSIZE)], size - total);
        brelse(buf);
        total = total + (size - total);
    }

    while (total < size) {
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(dest, buf->data, size - total);
        brelse(buf);
        zone++;
        total = total + (size - total);
    }
    return total;
}
u64 writei(struct inode *ip, char *src, u64 offset, u64 size) {
    u64 total = 0;
    int zone = 0;
    struct buf *buf;

    if ((ip->mode & S_IFMT) == S_IFCHR) {
        VERBOSE_PRINTK("writei: S_IFCHR\n");
        for (u64 i = 0; i < size; i++) {
            cdevsw[ip->dev.major].write(src[i]);
        }
        return size;
    }

    if (offset > 0) {
        for (u64 i = BLOCKSIZE; i <= offset; i += BLOCKSIZE) {
            zone++;
        }
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(&buf->data[(offset % BLOCKSIZE)], src, size - total);
        bwrite(buf);
        brelse(buf);
        ip->size += (size - total);
        total = total + (size - total);
    }
    while (total < size) {
        // TODO: allocate zone bit map.
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(buf->data, src, size - total);
        bwrite(buf);
        brelse(buf);
        zone++;
        ip->size += (size - total);
        total = total + (size - total);
    }
    iupdate(ip);

    return total;
}

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

static u64 alloc_bit(dev_t dev, int map) {
    struct super_block *sb;
    struct buf *buf;
    u64 inum;
    u8 pos;
    u64 last_block;
    u64 map_offset;

    sb = getfs(dev);
    if (map == 0) {
        // IMAP
        last_block = sb->imap_blocks;
        map_offset = IMAP(sb);
    } else if (map == 1) {
        // ZMAP
        last_block = sb->zmap_blocks;
        map_offset = ZMAP(sb);
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

static void free_bit(dev_t dev, int map, u64 pos) {
    struct super_block *sb;
    struct buf *buf;
    u64 blkoff;
    u64 byteoff;
    u64 bitoff;
    u64 map_offset;

    sb = getfs(dev);
    if (map == 0) {
        // IMAP
        map_offset = IMAP(sb);
        pos--;
    } else if (map == 1) {
        // ZMAP
        map_offset = ZMAP(sb);
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

static u8 has_bit(dev_t dev, int map, u64 n) {
    u64 blkoff;
    u64 byteoff;
    u8 bitoff;
    u8 byte;
    u8 bit;
    struct super_block *sb;
    struct buf *buf;

    sb = getfs(dev);
    if (map == 1) {
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

    inum = alloc_bit(dev, 0);
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
    offset =
        (IMAP(sb) + sb->imap_blocks + sb->zmap_blocks + (ip->inum / NINODE));
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
    char *save;
    char *basedir;
    char *filename;
    char *path;
    u64 offset = 0;
    struct direct dir;
    struct file *fp;

    // Remove const qualifier
    path = (char *)pathname;
    // Back up original path, since namei destroy it.
    save = kmalloc(strlen(path));
    strcpy(save, path);

    if (strncmp(path, ".", 2) == 0) {
        // TODO: CWD is not supported for now
        basedir = "/";
    } else {
        basedir = dirname(path);
        strcpy(path, save);
    }
    filename = basename(path);
    strcpy(path, save);

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
        // TODO: always trunc.
        ip->size = 0x0;
        struct super_block *sb;
        sb = getfs(ip->dev);
        ip->zone[0] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[1] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[2] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[3] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[4] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[5] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[6] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));
        ip->zone[7] = ((sb->first_data_zone) + alloc_bit(ip->dev, 1));

        // Update newly create inode.
        iupdate(ip);
        // Set newly created filename and its ino to empty direct entry.
        strcpy(dir.name, filename);
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
    fp->ip->uid = 0;
    fp->ip->gid = 0;
    fp->ip->atime = 0;
    fp->ip->mtime = 0;
    fp->ip->ctime = 0;
    if (flags & O_TRUNC) {
        fp->ip->size = 0;
        free_bit(fp->ip->dev, 1, fp->ip->zone[0]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[1]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[2]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[3]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[4]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[5]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[6]);
        free_bit(fp->ip->dev, 1, fp->ip->zone[7]);
        fp->ip->zone[0] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[1] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[2] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[3] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[4] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[5] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[6] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
        fp->ip->zone[7] = ((sb->first_data_zone) + alloc_bit(fp->ip->dev, 1));
    }
    iupdate(fp->ip);

free_and_exit:
    kfree(save);
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
