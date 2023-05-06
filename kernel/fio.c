#include <file.h>
#include <panic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <stddef.h>
#include <sys/stat.h>

struct file file[NFILE];

int ufalloc(void) {
    struct proc *rp;

    rp = this_proc();

    for (int i = 0; i < NOFILE; i++) {
        if (rp->ofile[i] == NULL) {
            return i;
        }
    }
    return -1;
}

struct file *falloc(void) {
    int fd;
    struct proc *rp;
    struct file *fp;

    rp = this_proc();
    fd = ufalloc();

    if (fd < 0) {
        return NULL;
    }

    for (int i = 0; i < NFILE; i++) {
        if (file[i].count == 0) {
            fp = &file[i];
            rp->ofile[fd] = fp;
            fp->count++;
            fp->offset = 0;
            fp->offset = 0;
            fp->ip = NULL;
            return fp;
        }
    }
    panic("no file\n");
    return NULL;
}

void openi(struct inode *ip) {
    switch (ip->mode & I_TYPE) {
        case I_CHAR_SPECIAL:
            if (cdevsw[major(ip->dev)].open != NULL) {
                cdevsw[major(ip->dev)].open();
            }
            break;
        case I_BLOCK_SPECIAL:
            if (bdevsw[major(ip->dev)].open != NULL) {
                bdevsw[major(ip->dev)].open();
            }
            break;
        default:
            panic("openi: unsupported device\n");
            break;
    }
}

void closei(struct inode *ip) {
    switch (ip->mode & I_TYPE) {
        case I_CHAR_SPECIAL:
            if (cdevsw[major(ip->dev)].close != NULL) {
                cdevsw[major(ip->dev)].close();
            }
            break;
        case I_BLOCK_SPECIAL:
            if (bdevsw[major(ip->dev)].close != NULL) {
                bdevsw[major(ip->dev)].close();
            }
            break;
        default:
            iput(ip);
            break;
    }
}
