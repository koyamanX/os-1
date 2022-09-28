#ifndef PROC_H
#define PROC_H

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
u64 mpid = 0;
struct proc {
	u64 stat;	
	u64 pid;
	context context;
	char name[16];
} proc[NPROCS];
#define UNUSED 0
#define RUNNING 1
#define RUNNABLE 2


#endif
