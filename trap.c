#include "riscv.h"
#include "os1.h"
#include "uart.h"
#include "proc.h"
#include "printk.h"

void usertrap(void) {
	asm volatile("nop");
}

void usertrapret(void) {
	struct proc *p;

	w_stvec(TRAMPOLINE);
	p = cpus[r_tp()].rp;
	p->tf->satp = r_satp();
	p->tf->ksp = (u64)(p->kstack + PAGE_SIZE);
    w_sstatus(((r_sstatus() & ~SSTATUS_SPP) | SSTATUS_SPIE));
	w_sepc(p->tf->sepc);
	
	((void(*)(u64))(TRAMPOLINE + (((u64) userret) - ((u64) trampoline))))((u64)SATP(p->pgtbl));
}

void kerneltrap(void) {
	u64 scause = r_scause();
	
	switch(scause) {
		case LOAD_PAGE_FAULT: {
			printk("Load page fault: epc:%x, tval:%x\n", r_sepc(), r_stval());
			break;
		}
		case STORE_AMO_PAGE_FAULT: {
			printk("store/amo page fault: epc:%x, tval:%x\n", r_sepc(), r_stval());
			break;
		}
		case INSTRUCTION_PAGE_FAULT: {
			printk("instruction page fault: epc:%x, tval:%x\n", r_sepc(), r_stval());
			break;
		}
		case LOAD_ACCESS_FAULT: {
			uart_puts("load access fault\n");
			break;
		}
#define SMODE_SOFTWARE_INTERRUPT 0x8000000000000001
		case SMODE_SOFTWARE_INTERRUPT: {
			uart_puts("software interrupt\n");
			w_sip(0x0);
			break;
		}
		default: {
			uart_puts("fault\n");
			break;
		}
	}

	return ;
}
