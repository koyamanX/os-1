#include "sched.h"
#include "proc.h"

void sched(void) {
	return ;
}

extern void swtch(context_t *old, context_t *new);

void scheduler(void) {
	while(1) {
		for(struct proc *p = &procs[0]; p < &procs[NPROCS]; p++) {
			if(p->stat != RUNNABLE)
				continue;
			cpus[r_tp()].rp = p;
			swtch(&cpus[r_tp()].ctx, &p->ctx);
			cpus[r_tp()].rp = NULL;
		}
	}
}
