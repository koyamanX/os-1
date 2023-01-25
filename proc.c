#include "riscv.h"
#include "os1.h"
#include "types.h"
#include "proc.h"
#include "vm.h"
#include "string.h"

struct proc procs[NPROCS];
u64 mpid = 1;

void initproc(void) {
	for(int i = 0; i < NPROCS; i++) {
		procs[i].stat = UNUSED;
	}
}

void init(void) {
	while(1) {
		asm volatile("nop");
	}
}

pagetable_t uvminit(void) {
	pagetable_t upgtbl;

	upgtbl = kalloc();
	memset(upgtbl, 0, PAGE_SIZE);
	kvmmap(upgtbl, (u64)TRAMPOLINE, (u64)trampoline, PAGE_SIZE, PTE_R|PTE_X|PTE_V);
	kvmmap(upgtbl, (u64)TRAPFRAME, (u64)kalloc(), PAGE_SIZE, PTE_R|PTE_W|PTE_V);
	kvmmap(upgtbl, (u64)0x0, (u64)init, PAGE_SIZE, PTE_R|PTE_W|PTE_X|PTE_V);

	return upgtbl;
}

void create_init(void) {
	procs[mpid].pid = mpid;
	procs[mpid].stat = RUNNABLE;
	strcpy(procs[mpid].name, "init");
//	memset(&procs[mpid].context, 0x0, 64*32);
	procs[mpid].pgtbl = uvminit();
	pte_t *pte = kvmwalk(procs[mpid].pgtbl, TRAPFRAME);
	trapframe_t *tf = (trapframe_t *)PTE2PA(*pte);
	memset(tf, 0, sizeof(trapframe_t));
	tf->satp = SATP(procs[mpid].pgtbl);
}

int newproc(void) {
	/* Set newly created process
	 * a0 <- pid
	 * ra <- ra
	 * SO, newly created process start executing
	 * in kernel, after newproc call.
	 * fork() will return to user mode, so kernel mode is ok
	 */

	return 0;
}


