#include "riscv.h"
#include "os1.h"
#include "types.h"
#include "proc.h"
#include "vm.h"
#include "string.h"

struct proc procs[NPROCS];
u64 mpid = 1;
struct cpu cpus[NCPUS];

void initcpu(void) {
	for(int i = 0; i < NCPUS; i++) {
		cpus[i].rp = NULL;
		memset(&cpus[i].ctx, 0, sizeof(context_t));
	}
}

void initproc(void) {
	mpid = 1;
	for(int i = 0; i < NPROCS; i++) {
		procs[i].stat = UNUSED;
	}
}

__attribute__ ((section(".userproc0")))
void init(void) {
	while(1) {
		asm volatile("nop");
	}
}

void userinit(void) {
	u64 pid = newproc();
	kvmmap(procs[pid].pgtbl, 0x0, (u64)init, PAGE_SIZE, PTE_V|PTE_W|PTE_R|PTE_X|PTE_U);
	kvmdump(procs[pid].pgtbl, TRAPFRAME);
	kvmdump(procs[pid].pgtbl, TRAMPOLINE);
	kvmdump(procs[pid].pgtbl, 0x0);
}

int newproc(void) {
	struct proc *p;

	p = &procs[mpid];
	p->stat = RUNNABLE;
	p->pid = mpid;
	p->tf = kalloc();
	p->pgtbl = kalloc();
	p->kstack = kalloc();
	memset(p->tf, 0, sizeof(trapframe_t));
	p->ctx.ra = (u64)usertrapret;
	p->ctx.sp = (u64)(p->kstack+PAGE_SIZE);
	p->tf->satp = SATP(p->pgtbl);	
	p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
	kvmmap(p->pgtbl, TRAPFRAME, (u64)p->tf, PAGE_SIZE, PTE_V|PTE_W|PTE_R);
	kvmmap(p->pgtbl, TRAMPOLINE, (u64)trampoline, PAGE_SIZE, PTE_V|PTE_X|PTE_R);
	
	return mpid++;
}


