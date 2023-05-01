#include <fs.h>
#include <proc.h>
#include <sys/stat.h>

int fstat(int fildes, struct stat *buf) {
    struct inode *ip;
    struct super_block *sb;
    struct file *fp;
    struct proc *rp;

    rp = this_proc();
    fp = rp->ofile[fildes];
    ip = fp->ip;
    if (ip == NULL) {
        return -1;
    }

    buf->st_dev = ip->dev;
    buf->st_ino = ip->inum;
    buf->st_mode = ip->mode;
    buf->st_nlink = ip->nlinks;
    buf->st_uid = ip->uid;
    buf->st_gid = ip->gid;
    // TODO: buf->st_rdev;
    buf->st_size = ip->size;
    buf->st_atime = ip->atime;
    buf->st_mtime = ip->mtime;
    buf->st_ctime = ip->ctime;
    sb = getfs(ip->dev);
    buf->st_blksize = sb->block_size;
    // TODO: st_blocks

    return 0;
}
