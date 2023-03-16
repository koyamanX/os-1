#include <panic.h>
#include <uart.h>

void panic(char *msg) {
    uart_puts("panic: ");
    uart_puts(msg);
    while (1) {
        asm volatile("nop");
    }
}
