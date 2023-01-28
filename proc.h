#ifndef PROC_H
#define PROC_H

#include "riscv.h"

typedef struct {
	u64 ra;
	u64 sp;
	u64 gp;
	u64 tp;
	u64 t0;
	u64 t1;
	u64 t2;
	u64 s0;
	u64 s1;
	u64 a0;
	u64 a1;
	u64 a2;
	u64 a3;
	u64 a4;
	u64 a5;
	u64 a6;
	u64 a7;
	u64 s2;
	u64 s3;
	u64 s4;
	u64 s5;
	u64 s6;
	u64 s7;
	u64 s8;
	u64 s9;
	u64 s10;
	u64 s11;
	u64 t3;
	u64 t4;
	u64 t5;
	u64 t6;
	u64 sepc;
	u64 satp;
	u64 ksp;
} trapframe_t;

typedef struct {
	u64 ra;
	u64 sp;
	u64 s0;
	u64 s1;
	u64 s2;
	u64 s3;
	u64 s4;
	u64 s5;
	u64 s6;
	u64 s7;
	u64 s8;
	u64 s9;
	u64 s10;
	u64 s11;
} context_t;

#define NPROCS 16
struct proc {
	u64 stat;	
	u64 pid;
	trapframe_t *tf;
	context_t ctx;
	char name[16];
	pagetable_t pgtbl;
	u8 *kstack;
};
#define UNUSED 0
#define USED	1
#define RUNNING 2
#define RUNNABLE 3

struct cpu {
	struct proc *rp;
	context_t ctx;
};
#define NCPUS 1
extern struct cpu cpus[NCPUS];

extern struct proc procs[NPROCS];
void initproc(void);
void initcpu(void);
pagetable_t uvminit(void);
int newproc(void);
void init(void);
void userinit(void);
extern void usertrapret(void);
extern void userret(pagetable_t pgtbl);

#endif
