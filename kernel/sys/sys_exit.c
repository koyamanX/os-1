#include <fs.h>
#include <proc.h>
#include <sched.h>

void _exit(int status) {
    struct proc *rp;

    rp = this_proc();

    for (int i = 0; i < NOFILE; i++) {
        if (rp->ofile[i] != NULL) {
            closei(rp->ofile[i]->ip);
            rp->ofile[i]->count--;
        }
    }

    rp->stat = ZOMBIE;
    wakeup(rp);
    sched();
}
