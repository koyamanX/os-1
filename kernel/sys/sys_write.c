#include <fcntl.h>
#include <fs.h>
#include <panic.h>
#include <proc.h>

ssize_t write(int fd, const void *buf, size_t count) {
    u64 ret = -1;
    struct proc *rp;
    struct file *fp;

    rp = this_proc();
    fp = rp->ofile[fd];
    if (fp == NULL) {
        panic("No file opened\n");
    }
    ret = writei(fp->ip, (char *)buf, fp->offset, count);
    fp->offset += count;

    return ret;
}
