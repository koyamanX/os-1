#include <file.h>
#include <fs.h>
#include <kmalloc.h>
#include <lib.h>
#include <libgen.h>
#include <sys/stat.h>

int mknod(const char *pathname, mode_t mode, dev_t dev) {
    struct inode *ip;
    struct direct dir;
    struct file *fp;
    int fd = -1;
    char *basec;
    char *dirc;
    char *basedir;
    char *filename;
    u64 offset;

    // Characters for basedir, since basedir may destroy original string.
    basec = kstrdup(pathname);
    // Characters for dirname, since dirname may destroy original string.
    dirc = kstrdup(pathname);

    basedir = dirname(dirc);
    filename = basename(basec);
    if (strncmp(basedir, ".", 2) == 0) {
        // TODO: CWD is not supported for now
        basedir = "/";
    }
    ip = namei(basedir);
    if (ip == NULL) {
        goto free_and_exit;
    }

    // Allocate file descriptor.
    fd = ufalloc();
    // Allocate file structure.
    fp = falloc();
    fp->flags = mode;
    // Allocate inode.
    fp->ip = ialloc(ip->dev);
    fp->ip->mode = mode & RWX_MODES;
    if (S_ISBLK(mode)) {
        fp->ip->mode |= I_BLOCK_SPECIAL;
    } else if (S_ISCHR(mode)) {
        fp->ip->mode |= I_CHAR_SPECIAL;
    }
    // Do device specific open.
    openi(fp->ip);
    fp->ip->nlinks++;
    fp->ip->size = 0x0;
    fp->ip->dev = dev;
    iupdate(fp->ip);

    // find free slot.
    offset = newdirect(ip);
    // Set newly created filename and its ino to empty direct entry.
    strcpy(dir.name, filename);
    // Inode number starts from 1.
    dir.ino = fp->ip->inum + 1;
    // Write direct entry.
    writei(ip, (char *)&dir, offset, sizeof(struct direct));

free_and_exit:
    kfree(basec);
    kfree(dirc);
    return fd;
}
