#include <elf.h>
#include <fs.h>
#include <kmalloc.h>
#include <lib.h>
#include <os1.h>
#include <panic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <trap.h>
#include <vm.h>

struct proc procs[NPROCS];
struct cpu cpu;
extern void init(void);

void initcpu(void) {
    cpu.rp = NULL;
    memset(&cpu.ctx, 0, sizeof(context_t));
}

void initproc(void) {
    for (int i = 0; i < NPROCS; i++) {
        procs[i].stat = UNUSED;
    }
}

void userinit(void) {
    struct proc *p;

    // Allocate new proc.
    p = newproc();

    // Load initcode at address 0.
    kvmmap(p->pgtbl, 4096, (u64)init, PAGE_SIZE,
           PTE_V | PTE_W | PTE_R | PTE_X | PTE_U);
    // Set parent to 0.
    p->ppid = 0;

    // Dump proc memory space.
    kvmdump(p->pgtbl, TRAPFRAME);
    kvmdump(p->pgtbl, TRAMPOLINE);
    kvmdump(p->pgtbl, 4096);
}

static u64 mpid = 1;
struct proc *newproc(void) {
    struct proc *p;

    for (p = &procs[0]; p < &procs[NPROCS]; p++) {
        if (p->stat == UNUSED) {
            p->stat = USED;
            p->pid = mpid++;
            goto found;
        }
    }
    return NULL;

found:
    // Initialize proc.
    p->stat = RUNNABLE;
    // Allocate memory for trapframe, page table, and kernel stack.
    p->tf = alloc_page();
    p->pgtbl = alloc_page();
    p->kstack = alloc_page();

    // Initialize trapframe.
    memset(p->tf, 0, sizeof(trapframe_t));
    p->tf->sepc = 4096;
    memset(p->pgtbl, 0, PAGE_SIZE);

    // Initialize context.
    p->ctx.ra = (u64)usertrapret;
    p->ctx.sp = (u64)(p->kstack + PAGE_SIZE);
    p->tf->satp = SATP(p->pgtbl);
    p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
    p->tf->trap_handler = (u64)(usertrap);
    p->tf->sp = USTACK;
    // Map trapframe and trampoline to proc's memory space.
    kvmmap(p->pgtbl, TRAPFRAME, (u64)p->tf, PAGE_SIZE, PTE_V | PTE_W | PTE_R);
    kvmmap(p->pgtbl, TRAMPOLINE, (u64)trampoline, PAGE_SIZE,
           PTE_V | PTE_X | PTE_R);

    for (u64 nstack = 1; nstack <= NUSTACK; nstack++) {
        u64 stack = USTACK - (nstack * PAGE_SIZE);
        kvmmap(p->pgtbl, stack, (u64)alloc_page(), PAGE_SIZE,
               PTE_V | PTE_W | PTE_R | PTE_U);
        kvmdump(p->pgtbl, stack);
    }
    p->heap = 0x20000000;

    return p;
}

void sleep(void *wchan) {
    struct proc *rp = this_proc();

    rp->wchan = wchan;
    rp->stat = SLEEP;

    sched();

    rp->wchan = NULL;
}

void wakeup(void *wchan) {
    for (struct proc *rp = &procs[0]; rp < &procs[NPROCS]; rp++) {
        if (rp->wchan == wchan && rp->stat == SLEEP) {
            rp->stat = RUNNABLE;
            rp->wchan = NULL;
        }
    }
}
