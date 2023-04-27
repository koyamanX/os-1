#include <alloc.h>
#include <buf.h>
#include <fs.h>
#include <os1.h>
#include <plic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <sched.h>
#include <string.h>
#include <sys/types.h>
#include <timer.h>
#include <uart.h>
#include <virtio.h>
#include <vm.h>

__attribute__((aligned(16))) char stack[PAGE_SIZE * 4];

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
    // Calling sret enables interrupt.
    w_sstatus(r_sstatus() | SSTATUS_SPIE);
    w_sie(SIE_SEIE | SIE_STIE | SIE_SSIE);

    asm volatile("mret");
}

void kmain(void) {
    pagetable_t kpgtbl;

    uart_init();
    INFO_PRINTK("kernel starts\n");
    kpgtbl = kvminit();
    kvmstart(kpgtbl);
    INFO_PRINTK("enable paging\n");
    initcpu();
    initproc();
    virtio_init();
    plic_init();
    binit();
    fsinit();
    userinit();
    scheduler();
}
