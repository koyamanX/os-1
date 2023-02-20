#include "riscv.h"
#include "os1.h"
#include "types.h"
#include "proc.h"
#include "vm.h"
#include "trap.h"
#include "string.h"
#include "sched.h"
#include "fs.h"
#include "printk.h"

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

extern void init(void);
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
	p->tf->trap_handler = (u64)(kerneltrap);
	kvmmap(p->pgtbl, TRAPFRAME, (u64)p->tf, PAGE_SIZE, PTE_V|PTE_W|PTE_R);
	kvmmap(p->pgtbl, TRAMPOLINE, (u64)trampoline, PAGE_SIZE, PTE_V|PTE_X|PTE_R);
	
	return mpid++;
}

int exec(const char *file, char const **argv) {
	struct inode *ip;
	char path[128];

	strcpy(path, file);
	ip = namei((char *)path);
	if(ip == NULL) {
		return -1;
	}
	char magic[5];
	readi(ip, (char *)magic, 0, 4);
	magic[4] = '\0';
	printk("exec: %s\n", magic);
	return 0;
}


