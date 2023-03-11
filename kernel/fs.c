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
#include <riscv.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <virtio.h>
#include <vm.h>

struct inode inode[NICACHE];
struct super_block sb[NSUPERBLK];

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
    struct buf *buf;
    u64 blkno;
    struct super_block *sb;
    struct inode *ip;

    sb = getfs(dev);
    /**
     * Inode number starts from 1, however block number starts with 0,
     * so subtract by 1 to map block number.
     */
    inum--;
    blkno = (IMAP(sb) + sb->imap_blocks + sb->zmap_blocks + (inum / NINODE));
    buf = bread(dev, blkno);

    for (int i = 0; i < NICACHE; i++) {
        // Find unused inode cache entry.
        if (inode[i].count == 0) {
            // Read inode to inode cache.
            memcpy(&inode[i], (buf->data + ((inum % NINODE) * INODE_SIZE)),
                   INODE_SIZE);
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
    u8 zone = 0;

    // TODO: find directory in all zone.
    buf = bread(rootdev, zmap(ip, zone));
    dp = (struct direct *)buf->data;
    for (int i = ip->size; i; i -= sizeof(struct direct)) {
        if (strcmp(dp->name, name) == 0) {
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

    if (*path == '/') {
        ip = iget(rootdev, ROOT);
        path++;
    }

    name = strtok(path, "/");
    if (name) {
        p = diri(ip, name);
        if (p != NULL) {
            ip = p;
        }
    }
    while ((name = strtok(NULL, "/")) != NULL) {
        p = diri(ip, name);
        if (p != NULL) {
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

    if ((ip->mode & S_IFMT) == S_IFCHR) {
        VERBOSE_PRINTK("readi: S_IFCHR\n");
        for (u64 i = 0; i < size; i++) {
            dest[i] = cdevsw[ip->dev.major].read();
        }
        return size;
    }

    if (offset > 0) {
        for (u64 i = BLOCKSIZE; i <= offset; i += BLOCKSIZE) {
            zone++;
        }
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(dest, &buf->data[(offset % BLOCKSIZE)], size - total);
        total = total + (size - total);
    }

    while (total < size) {
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(dest, buf->data, size - total);
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
        total = total + (size - total);
    }
    while (total < size) {
        // TODO: allocate zone bit map.
        buf = bread(rootdev, zmap(ip, zone));
        memcpy(buf->data, src, size - total);
        bwrite(buf);
        zone++;
        total = total + (size - total);
    }
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

struct inode *ialloc(dev_t dev) {
    struct super_block *sb;
    struct inode *ip = NULL;
    u64 offset;
    u8 pos;
    struct buf *buf;

    sb = getfs(dev);
    offset = IMAP(sb);

    for (u64 i = 0; i < (sb->imap_blocks / sizeof(u8)); i++) {
        buf = bread(dev, offset);
        if ((pos = ffs(~buf->data[i] & 0xff)) != 0) {
            buf->data[i] = buf->data[i] | (1 << (pos - 1));
            bwrite(buf);
            ip = iget(dev, i * 8 + (7 - pos));
            break;
        }
        offset++;
    }
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

    if (zone >= 8) {
        panic("zmap: Indirect zone is not supported\n");
    }
    addr = ip->zone[zone];

    return addr;
}
int open(const char *pathname, int flags, mode_t mode) {
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
    if (flags & O_CREAT) {
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
        fp->ip->mode = mode;
        fp->ip->nlinks++;
        fp->ip->size = 0x0;
        iupdate(fp->ip);
        strcpy(dir.name, filename);
        dir.ino = fp->ip->inum + 1;
        VERBOSE_PRINTK("%s:%x\n", dir.name, dir.ino);
        VERBOSE_PRINTK("%x\n", offset);
        offset -= sizeof(struct direct);
        writei(ip, (char *)&dir, offset, sizeof(struct direct));
    }

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
