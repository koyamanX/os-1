#include <kmalloc.h>
#include <proc.h>
#include <sleeplock.h>
#include <sys/wait.h>
#include <vm.h>

int waitpid(int pid, int *status, int options) {
    struct proc *rp = NULL;

    // pid == -1 is not supported.
    if (pid == -1) {
        return -1;
    }

    for (struct proc *p = &procs[0]; p < &procs[NPROCS]; p++) {
        if (p->pid == pid) {
            rp = p;
            break;
        }
    }
    if (rp == NULL) {
        return -1;
    }

    if (rp->stat == UNUSED) {
        return -1;
    }

    if (options == WNOHANG && rp->stat != ZOMBIE) {
        return -1;
    }

    if (rp->stat != ZOMBIE) {
        sleep(rp);
    }

    if (status != NULL) {
        *status = rp->tf->a0;
    }

    if (rp->stat == ZOMBIE) {
        kvmunmap(rp->pgtbl, 0x0, 0x80000000);
        // kvmunmap(rp->pgtbl, TRAPFRAME, PAGE_SIZE);
        if (rp->pgtbl) {
            // CAUTION: kfree is not implemented.
            kfree(rp->pgtbl);
            rp->pgtbl = NULL;
        }
        if (rp->kstack) {
            // CAUTION: kfree is not implemented.
            kfree(rp->kstack);
            rp->kstack = NULL;
        }
        rp->tf = NULL;
        rp->stat = UNUSED;
    }

    return pid;
}
