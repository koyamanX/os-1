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
#include "fs.h"

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

	char *buf = kalloc();
	memset(buf, 0, 512);
	virtio_req(buf, 2, 0);
	struct super_block *sb;
	sb = (struct super_block *)buf;
	printk("ninodes: %x\n", sb->ninodes);
	printk("nzones: %x\n", sb->nzones);
	printk("imap_blocks: %x\n", sb->imap_blocks);
	printk("zmap_blocks: %x\n", sb->zmap_blocks);
	printk("firstdatazone: %x\n", sb->firstdatazone);
	printk("log_zone_size: %x\n", sb->log_zone_size);
	printk("max_size: %x\n", sb->max_size);
	printk("zones: %x\n", sb->zones);
	printk("magic: %x\n", sb->magic);
	printk("block_size: %x\n", sb->block_size);
	printk("disk_version: %x\n", sb->disk_version);

	userinit();
	userinit();
	scheduler();

	while(1) {
		asm volatile("nop");
	}
}

