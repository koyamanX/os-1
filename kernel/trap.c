#include <os1.h>
#include <panic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <sched.h>
#include <stddef.h>
#include <sys/types.h>
#include <trap.h>
#include <uart.h>
#include <vm.h>

void usertrap(void) {
    asm volatile("nop");
}

void usertrapret(void) {
    struct proc *p;

    w_stvec(TRAMPOLINE);
    p = this_proc();
    p->tf->satp = r_satp();
    p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
    w_sstatus(((r_sstatus() & ~SSTATUS_SPP) | SSTATUS_SPIE));
    w_sepc(p->tf->sepc);

    ((void (*)(u64))(TRAMPOLINE + (((u64)userret) - ((u64)trampoline))))(
        (u64)SATP(p->pgtbl));
}

int syscall(struct proc *rp);
void kerneltrap(void) {
    struct proc *rp;
    rp = this_proc();
    u64 scause = r_scause();

    switch (scause) {
        case LOAD_PAGE_FAULT: {
            DEBUG_PRINTK("Load page fault: epc:%x, tval:%x\n", r_sepc(),
                         r_stval());
            break;
        }
        case STORE_AMO_PAGE_FAULT: {
            DEBUG_PRINTK("store/amo page fault: epc:%x, tval:%x\n", r_sepc(),
                         r_stval());
            break;
        }
        case INSTRUCTION_PAGE_FAULT: {
            DEBUG_PRINTK("instruction page fault: epc:%x, tval:%x\n", r_sepc(),
                         r_stval());
            break;
        }
        case LOAD_ACCESS_FAULT: {
            DEBUG_PRINTK("load access fault: epc:%x, tval:%x\n", r_sepc(),
                         r_stval());
            break;
        }
#define SMODE_SOFTWARE_INTERRUPT 0x8000000000000001
        case SMODE_SOFTWARE_INTERRUPT: {
            w_sip(0x0);
            rp->stat = RUNNABLE;
            sched();
            break;
        }
#define ECALL_FROM_U_MODE 8
        case ECALL_FROM_U_MODE: {
            rp->tf->sepc += 4;
            rp->tf->a0 = syscall(rp);
            sched();
            break;
        }
        default: {
            DEBUG_PRINTK("trap: fault: cause: %x, epc:%x, tval:%x\n",
                         r_scause(), r_sepc(), r_stval());
            panic("trap");
            break;
        }
    }
    usertrapret();
}
