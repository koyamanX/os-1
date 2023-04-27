#include <printk.h>
#include <proc.h>
#include <sched.h>

extern void swtch(context_t *old, context_t *new);

void sched(void) {
    swtch(&this_proc()->ctx, &this_cpu().ctx);
}

void scheduler(void) {
    struct proc *rp = &procs[0];

    while (1) {
        if (rp >= &procs[NPROCS]) {
            rp = &procs[0];
        }
        if (rp->stat != RUNNABLE) {
            rp++;
            continue;
        }
        VERBOSE_PRINTK("scheduler: pid==%x\n", rp->pid);
        this_proc() = rp;
        rp->stat = RUNNING;
        swtch(&this_cpu().ctx, &rp->ctx);
        this_proc() = NULL;
        if (rp->stat == RUNNING) {
            rp->stat = RUNNABLE;
        }
        rp++;
    }
}
