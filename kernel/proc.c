#include <elf.h>
#include <fs.h>
#include <os1.h>
#include <panic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <sched.h>
#include <slob.h>
#include <string.h>
#include <sys/types.h>
#include <trap.h>
#include <vm.h>

struct proc procs[NPROCS];
struct cpu cpus[NCPUS];
extern void init(void);

void initcpu(void) {
    for (int i = 0; i < NCPUS; i++) {
        cpus[i].rp = NULL;
        memset(&cpus[i].ctx, 0, sizeof(context_t));
    }
}

void initproc(void) {
    for (int i = 0; i < NPROCS; i++) {
        procs[i].stat = UNUSED;
    }
}

void userinit(void) {
    u64 pid;

    // Allocate new proc.
    pid = newproc();

    // Load initcode at address 0.
    kvmmap(procs[pid].pgtbl, 4096, (u64)init, PAGE_SIZE,
           PTE_V | PTE_W | PTE_R | PTE_X | PTE_U);

    // Dump proc memory space.
    kvmdump(procs[pid].pgtbl, TRAPFRAME);
    kvmdump(procs[pid].pgtbl, TRAMPOLINE);
    kvmdump(procs[pid].pgtbl, 4096);
}

int newproc(void) {
    struct proc *p;
    u64 pid = 1;

    // Find unused proc from procs.
    for (pid = 1; pid < NPROCS; pid++) {
        if (procs[pid].stat == UNUSED) {
            p = &procs[pid];
            goto found;
        }
    }
    return -1;

found:
    // Initialize proc.
    p->stat = RUNNABLE;
    p->pid = pid;
    // Allocate memory for trapframe, page table, and kernel stack.
    p->tf = alloc_page();
    p->pgtbl = alloc_page();
    p->kstack = alloc_page();

    // Initialize trapframe.
    memset(p->tf, 0, sizeof(trapframe_t));
    p->tf->sepc = 4096;

    // Initialize context.
    p->ctx.ra = (u64)usertrapret;
    p->ctx.sp = (u64)(p->kstack + PAGE_SIZE);
    p->tf->satp = SATP(p->pgtbl);
    p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
    p->tf->trap_handler = (u64)(kerneltrap);
    p->tf->sp = 4096 + PAGE_SIZE;
    // Map trapframe and trampoline to proc's memory space.
    kvmmap(p->pgtbl, TRAPFRAME, (u64)p->tf, PAGE_SIZE, PTE_V | PTE_W | PTE_R);
    kvmmap(p->pgtbl, TRAMPOLINE, (u64)trampoline, PAGE_SIZE,
           PTE_V | PTE_X | PTE_R);

    return pid;
    ;
}

int exec(const char *file, char const **argv) {
    struct inode *ip;
    Elf64_Ehdr ehdr;
    struct proc *rp;
    char *seg;
    Elf64_Phdr *phdr = NULL;

    ip = namei((char *)file);
    // TODO: check permission for executable file (rx)
    if (ip == NULL) {
        return -1;
    }
    readi(ip, (char *)&ehdr, 0, sizeof(ehdr));

    if (IS_RISCV_ELF(ehdr) && ehdr.e_type == ET_EXEC) {
        DEBUG_PRINTK("Valid ELF\n");
    } else {
        DEBUG_PRINTK("Invalid ELF\n");
    }

    if (ehdr.e_phnum > 4) {
        panic("exec: load failed\n");
    }
    phdr = kmalloc(sizeof(Elf64_Phdr));

    rp = this_proc();
    readi(ip, (char *)phdr, ehdr.e_phoff, sizeof(Elf64_Phdr) * ehdr.e_phnum);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            INFO_PRINTK(
                "PT_LOAD: p_offset: %x, p_vaddr: %x, p_paddr: %x, p_filesz: "
                "%x,p_memsz: %x, p_align: %x\n",
                phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_paddr,
                phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align);
            seg = (char *)va2pa(rp->pgtbl, 4096);
            memset(seg, 0, PAGE_SIZE);
            readi(ip, (char *)seg, phdr[i].p_offset, PAGE_SIZE);
            sfence_vma();  // required?
            break;         // TODO: only load first page for segment
        }
    }

    kfree(phdr);

    rp->tf->sepc = ehdr.e_entry;
    rp->tf->sp = 4096 + PAGE_SIZE;

    return 0;
}

// TODO: pid_t
int fork(void) {
    int pid;
    struct proc *child;
    struct proc *parent;

    // Allocate new proc.
    pid = newproc();
    child = &procs[pid];
    if ((child == NULL) || (pid == -1)) {
        return -1;
    }
    // Get parent proc.
    parent = this_proc();
    // Copy trapframe and kernel stack.
    memcpy(child->tf, parent->tf, sizeof(trapframe_t));
    memcpy(child->kstack, parent->kstack, PAGE_SIZE);
    // Set return value for child.
    child->tf->a0 = 0;
    // Set return value for parent.
    parent->tf->a0 = pid;
    // Copy memory space.
    uvmcopy(child->pgtbl, parent->pgtbl, PAGE_SIZE * 2);
    // Copy open files.
    memcpy((char *)child->ofile, (char *)parent->ofile, sizeof(parent->ofile));

    return pid;
}

void _exit(int status) {
    struct proc *rp;

    rp = this_proc();

    for (int i = 0; i < NOFILE; i++) {
        if (rp->ofile[i] != NULL) {
            closei(rp->ofile[i]->ip);
            rp->ofile[i]->count--;
        }
    }

    // TODO: stat should be zombie?
    rp->stat = UNUSED;

#if 0
    // TODO: Release all of memory used by process, for now, just free address
    // 0.
    uvmunmap(rp->pgtbl, 0, PAGE_SIZE);
    uvmunmap(rp->pgtbl, TRAPFRAME, PAGE_SIZE);
    //uvmunmap(rp->pgtbl, TRAMPOLINE, PAGE_SIZE);
    free_page(rp->kstack);
    free_page(rp->tf);
    free_page(rp->pgtbl);
#endif 
}
