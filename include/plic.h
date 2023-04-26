#ifndef _PLIC_H
#define _PLIC_H

// https://github.com/qemu/qemu/blob/c3f9aa8e488db330197c9217e38555f6772e8f07/include/hw/riscv/virt.h#L89
#define UART_IRQ 10

#define PLIC_BASE_ADDR 0x0c000000
#define PLIC_MPRIORITY_BASE_ADDR 0x0c200000
#define PLIC_SPRIORITY_BASE_ADDR 0x0c201000
#define PLIC_PENDING_BASE_ADDR 0x0c001000
#define PLIC_MENABLE_BASE_ADDR 0x0c002000
#define PLIC_SENABLE_BASE_ADDR 0x0c002080
#define PLIC_MCLAIM_COMPLETE_BASE_ADDR 0x0c200004
#define PLIC_SCLAIM_COMPLETE_BASE_ADDR 0x0c201004

void plic_init(void);
int plic_claim(void);
void plic_complete(int claim);

#endif /* _PLIC_H */
