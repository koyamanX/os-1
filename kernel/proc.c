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

static u64 mpid = 0;
struct proc *newproc(void) {
    struct proc *p;

	for(p = &procs[0]; p < &procs[NPROCS]; p++){
		if(p->stat == UNUSED) {
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

    return p;
}

int exec(const char *file, char const **argv) {
    struct inode *ip;
    Elf64_Ehdr ehdr;
    struct proc *rp;
    char *page;
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

    phdr = kmalloc(sizeof(Elf64_Phdr));

    rp = this_proc();
    readi(ip, (char *)phdr, ehdr.e_phoff, sizeof(Elf64_Phdr) * ehdr.e_phnum);

    u64 prot = PTE_V | PTE_U;
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            u64 off = phdr[i].p_offset;

            VERBOSE_PRINTK(
                "PT_LOAD: p_offset: %x, p_vaddr: %x, p_paddr: %x, p_filesz: "
                "%x,p_memsz: %x, p_align: %x\n",
                phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_paddr,
                phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align);

            if (phdr[i].p_flags & PF_X) {
                prot |= PTE_X;
            }
            if (phdr[i].p_flags & PF_R) {
                prot |= PTE_R;
            }
            if (phdr[i].p_flags & PF_W) {
                prot |= PTE_W;
            }
            for (u64 va = phdr[i].p_vaddr;
                 va < phdr[i].p_vaddr + phdr[i].p_memsz; va += PAGE_SIZE) {
                page = (char *)va2pa(rp->pgtbl, va);
                if (page == NULL) {
                    page = alloc_page();
                    kvmmap(rp->pgtbl, va, (u64)page, PAGE_SIZE, prot);
                    kvmdump(rp->pgtbl, va);
                }
                readi(ip, (char *)page, off, PAGE_SIZE);
                off += PAGE_SIZE;
            }
            // TODO: If memsz is grater than filesz, zero fill.
        }
    }
    kfree(phdr);

    rp->tf->sepc = ehdr.e_entry;
    rp->tf->sp = USTACK;

    return 0;
}

// TODO: pid_t
int fork(void) {
    struct proc *child;
    struct proc *parent;

    // Allocate new proc.
    child = newproc();
    if ((child == NULL)) {
		return -1;
    }
    // Get parent proc.
    parent = this_proc();
    // Set parent pid.
    child->ppid = parent->pid;
    // Copy trapframe and kernel stack.
    memcpy(child->tf, parent->tf, sizeof(trapframe_t));
    memcpy(child->kstack, parent->kstack, PAGE_SIZE);
    // Set return value for child.
    child->tf->a0 = 0;
    // Copy memory space.
    uvmcopy(child->pgtbl, parent->pgtbl, 0x80000000);
    // Copy open files.
    memcpy((char *)child->ofile, (char *)parent->ofile, sizeof(parent->ofile));

    return child->pid;
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

    rp->stat = ZOMBIE;
    wakeup(rp);
    sched();
}

void *sbrk(intptr_t increment) {
    struct proc *rp;
    void *addr;

    rp = this_proc();
    addr = (void *)rp->heap;
    for (u64 i = 0; i < ROUNDUP(increment); i -= PAGE_SIZE) {
        kvmmap(rp->pgtbl, rp->heap + i, (u64)alloc_page(), PAGE_SIZE,
               PTE_V | PTE_W | PTE_R | PTE_X | PTE_U);
        kvmdump(rp->pgtbl, rp->heap + i);
    }
    rp->heap = ROUNDUP(increment);

    return addr;
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

int waitpid(int pid, int *status, int options) {
    struct proc *rp = NULL;

    // pid == -1 is not supported.
    if (pid == -1) {
        return -1;
    }

    for (struct proc *p = &procs[0]; p < &procs[NPROCS]; p++) {
        if (p->pid == pid) {
            rp = p;
            break;
        }
    }
    if (rp == NULL) {
        return -1;
    }

    if (rp->stat == UNUSED) {
        return -1;
    }

    if (options == WNOHANG && rp->stat != ZOMBIE) {
        return -1;
    }

    if (rp->stat != ZOMBIE) {
        sleep(rp);
    }

    if (status != NULL) {
        *status = rp->tf->a0;
    }

    if (rp->stat == ZOMBIE) {
        kvmunmap(rp->pgtbl, 0x0, 0x80000000);
        // kvmunmap(rp->pgtbl, TRAPFRAME, PAGE_SIZE);
        if (rp->pgtbl) {
            // CAUTION: kfree is not implemented.
            kfree(rp->pgtbl);
            rp->pgtbl = NULL;
        }
        if (rp->kstack) {
            // CAUTION: kfree is not implemented.
            kfree(rp->kstack);
            rp->kstack = NULL;
        }
        rp->tf = NULL;
        rp->stat = UNUSED;
    }

    return pid;
}

int brk(void *addr) {
    this_proc()->heap = ROUNDUP((u64)addr);

    return 0;
}
