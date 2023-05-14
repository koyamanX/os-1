#include <fcntl.h>
#include <fs.h>
#include <kmalloc.h>
#include <lib.h>
#include <libgen.h>
#include <printk.h>
#include <proc.h>
#include <sys/stat.h>

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
        // Allocate new inode.
        ip = ialloc(dip->dev);
        // Set mode and size.
        ip->mode = mode | I_REGULAR;
        // Always trunc for newly created file.
        // Zone is expanded and allocated in write system call.
        ip->size = 0x0;

        // find free slot.
        offset = newdirect(dip);
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
    if (flags & O_APPEND) {
        fp->offset = fp->ip->size;
    }
    // Update inode.
    iupdate(fp->ip);

free_and_exit:
    kfree(path);
    kfree(basec);
    kfree(dirc);
    return fd;
}
