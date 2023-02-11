#include "riscv.h"
#include "proc.h"
#include "uart.h"
#include "types.h"
#include "vm.h"
#include "string.h"
#include "printk.h"
#include "os1.h"
#include "timer.h"
#include "sched.h"
#include "virtio.h"

__attribute__ ((aligned (16))) char stack[PAGE_SIZE*4];

void kinit(void) {
	u64 mstatus;

	mstatus = r_mstatus();
	mstatus &= MSTATUS_MPP_MASK;
	mstatus |= MSTATUS_MPP_S_MODE;
	w_mstatus(mstatus);

	w_satp(0);

	init_timer();

	w_mepc((u64)((u64 *)&kmain));

	w_stvec((u64)((u64 *)&kernelvec));
	w_sstatus(r_sstatus() | 1<<1);

	asm volatile("mret");
}

void kmain(void) {
	pagetable_t kpgtbl;

	uart_init();
	printk("kernel starts\n");
	kpgtbl = kvminit();
	kvmstart(kpgtbl);
	printk("enable paging\n");
	initcpu();
	initproc();
	virtio_init();
	userinit();
	userinit();
	int len = strlen("chinchin");
	printk("len:%x\n",len);
	scheduler();

	while(1) {
		asm volatile("nop");
	}
}

