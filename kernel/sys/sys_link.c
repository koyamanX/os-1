#include <fs.h>
#include <kmalloc.h>
#include <lib.h>
#include <libgen.h>
#include <proc.h>

// TODO: Update mtime and ctime on successful link.
// TODO: Check permissions.
// TODO: Support CWD.
int link(const char *path1, const char *path2) {
    int fd = -1;
    struct inode *p1;
    struct inode *dip;
    char *basec;
    char *dirc;
    char *basedir;
    char *filename;
    struct file *fp;
    struct direct dir;
    u64 offset = 0;

    basec = kstrdup(path2);
    dirc = kstrdup(path2);

    p1 = namei(path1);
    if (p1 == NULL) {
        goto free_and_exit;
    }
    if (p1->mode & I_DIRECTORY) {
        // If p1 is directory, link fails.
        goto free_and_exit;
    }
    basedir = dirname(dirc);
    filename = basename(basec);
    dip = namei(basedir);
    if (dip == NULL) {
        goto free_and_exit;
    }

    fd = ufalloc();
    fp = falloc();
    fp->flags = p1->mode;
    fp->ip = p1;
    fp->ip->nlinks++;

    // find free slot.
    offset = newdirect(dip);
    // Set newly created filename and its ino to empty direct entry.
    strcpy(dir.name, filename);
    // Inode number starts from 1.
    dir.ino = fp->ip->inum + 1;
    // Write direct entry.
    writei(dip, (char *)&dir, offset, sizeof(struct direct));

free_and_exit:
    kfree(basec);
    kfree(dirc);
    return fd;
}
