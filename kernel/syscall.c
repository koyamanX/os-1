#include <fcntl.h>
#include <panic.h>
#include <proc.h>
#include <sched.h>
#include <stddef.h>
#include <sys/stat.h>
#include <syscall.h>
#include <uart.h>
#include <unistd.h>
#include <vm.h>

int syscall(struct proc *rp) {
    u64 syscall_num = rp->tf->a7;
    u64 a0 = rp->tf->a0;
    u64 a1 = rp->tf->a1;
    u64 a2 = rp->tf->a2;
    int ret = -1;

    switch (syscall_num) {
        case __NR_WRITE:
            ret = write(a0, (void *)va2pa(rp->pgtbl, a1), a2);
            break;
        case __NR_EXECEV:
            ret = exec(((const char *)va2pa(rp->pgtbl, a0)),
                       ((char const **)va2pa(rp->pgtbl, a1)));
            break;
        case __NR_OPEN:
            ret = open(((const char *)va2pa(rp->pgtbl, a0)), a1, a2);
            break;
        case __NR_MKDIR:
            ret = mkdir(((const char *)va2pa(rp->pgtbl, a0)), a1);
            break;
        case __NR_MKNOD:
            dev_t dev;
            dev.major = (a2 >> 16) & 0xffff;
            dev.minor = a2 & 0xffff;
            ret = mknod(((const char *)va2pa(rp->pgtbl, a0)), a1, dev);
            break;
        case __NR_DUP:
            ret = dup((int)a0);
            break;
        case __NR_READ:
            ret = read(a0, (void *)va2pa(rp->pgtbl, a1), a2);
            break;
        case __NR_FORK:
            ret = fork();
            break;
        case __NR__EXIT:
            _exit(a0);
            extern void swtch(context_t * old, context_t * new);
            swtch(&rp->ctx, &cpus[r_tp()].ctx);
            break;
        case __NR_CLOSE:
            ret = close(a0);
            break;
        case __NR_LINK:
            ret = link((void *)va2pa(rp->pgtbl, a0),
                       (void *)va2pa(rp->pgtbl, a1));
            break;
        default:
            panic("invalid syscall\n");
            break;
    }

    return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
    u64 ret = -1;
    struct proc *rp;
    struct file *fp;

    rp = cpus[r_tp()].rp;
    fp = rp->ofile[fd];
    // TODO: fp->offset
    if (fp == NULL) {
        panic("No file opened\n");
    }
    ret = writei(fp->ip, (char *)buf, fp->offset, count);
    fp->offset += count;

    return ret;
}
ssize_t read(int fd, const void *buf, size_t count) {
    u64 ret = -1;
    struct proc *rp;
    struct file *fp;

    rp = cpus[r_tp()].rp;
    fp = rp->ofile[fd];
    ret = readi(fp->ip, (char *)buf, fp->offset, count);
    fp->offset += count;

    return ret;
}

int dup(int fildes) {
    int fd;
    struct proc *rp;
    struct file *fp;

    rp = cpus[r_tp()].rp;
    fp = rp->ofile[fildes];
    fd = ufalloc();
    rp->ofile[fd] = fp;

    return fd;
}
