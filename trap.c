#include "riscv.h"
#include "os1.h"
#include "uart.h"
#include "proc.h"

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
	
	userret(p->pgtbl);
}

void kerneltrap(void) {
	u64 scause = r_scause();
	
	switch(scause) {
		case LOAD_PAGE_FAULT: {
			uart_puts("load page fault\n");
			break;
		}
		case STORE_AMO_PAGE_FAULT: {
			uart_puts("store/amo page fault\n");
			break;
		}
		case INSTRUCTION_PAGE_FAULT: {
			uart_puts("instruction page fault\n");
			break;
		}
		case LOAD_ACCESS_FAULT: {
			uart_puts("load access fault\n");
			break;
		}
		default: {
			uart_puts("fault\n");
			break;
		}
	}

	return ;
}
