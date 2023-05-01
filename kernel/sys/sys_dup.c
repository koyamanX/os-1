#include <fs.h>
#include <proc.h>

int dup(int fildes) {
    int fd;
    struct proc *rp;
    struct file *fp;

    rp = this_proc();
    fp = rp->ofile[fildes];
    fd = ufalloc();
    rp->ofile[fd] = fp;

    return fd;
}
