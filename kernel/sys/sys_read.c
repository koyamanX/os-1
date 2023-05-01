#include <fs.h>
#include <proc.h>

ssize_t read(int fd, const void *buf, size_t count) {
    u64 ret = -1;
    struct proc *rp;
    struct file *fp;

    rp = this_proc();
    fp = rp->ofile[fd];
    ret = readi(fp->ip, (char *)buf, fp->offset, count);
    fp->offset += count;

    return ret;
}
