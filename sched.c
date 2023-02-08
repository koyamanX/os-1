#include "sched.h"
#include "proc.h"
#include "printk.h"

extern void swtch(context_t *old, context_t *new);

void sched(struct proc *rp) {
	w_sie(SIE_SEIE | SIE_STIE | SIE_SSIE);
	swtch(&rp->ctx, &cpus[r_tp()].ctx);
}

void scheduler(void) {
	struct proc *rp = &procs[0];

	while(1) {
		if(rp >= &procs[NPROCS]) {
			rp = &procs[0];
		}
		if(rp->stat != RUNNABLE) {
			rp++;
			continue;
		}
		printk("scheduler: pid==%x\n", rp->pid);
		cpus[r_tp()].rp = rp;
		rp->stat = RUNNING;
		swtch(&cpus[r_tp()].ctx, &rp->ctx);
		cpus[r_tp()].rp = NULL;
		rp->stat = RUNNABLE;
		rp++;
	}
}
