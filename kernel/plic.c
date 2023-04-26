#include <plic.h>
#include <uart.h>

void plic_init(void) {
    *((volatile unsigned int *)PLIC_BASE_ADDR + UART_IRQ) = 1;
    *((volatile unsigned int *)PLIC_SENABLE_BASE_ADDR) = 0x1 << UART_IRQ;
    *((volatile unsigned int *)PLIC_SPRIORITY_BASE_ADDR) = 0;
}

int plic_claim(void) {
    return *((volatile unsigned int *)PLIC_SCLAIM_COMPLETE_BASE_ADDR);
}

void plic_complete(int claim) {
    *((volatile unsigned int *)PLIC_SCLAIM_COMPLETE_BASE_ADDR) = claim;
}
