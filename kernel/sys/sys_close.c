#include <fs.h>
#include <panic.h>
#include <proc.h>

int close(int fd) {
    struct proc *rp;
    struct inode *ip;
    struct file *fp;

    rp = this_proc();
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
