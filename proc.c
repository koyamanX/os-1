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

void init(void) {
	while(1) {
		asm volatile("nop");
	}
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
	p->tf->ra = (u64)usertrapret;
	p->tf->satp = SATP(p->pgtbl);	
	kvmmap(p->pgtbl, TRAPFRAME, (u64)p->tf, PAGE_SIZE, PTE_V|PTE_W|PTE_R);
	kvmmap(p->pgtbl, TRAMPOLINE, TRAMPOLINE, PAGE_SIZE, PTE_V|PTE_X|PTE_R);
	
	return mpid++;
}


