#include <uart.h>
#include <sys/types.h>

void uart_init(void) {
	// Disable interrupt
	*UART_IER = 0;
	// 38.4 K baud rate
	*UART_LCR = UART_LCR_DLAB;
	*UART_DLL = 0x3;
	*UART_DLM = 0x0;

	// 8 bits char
	*UART_LCR = 0x3;
	// clear and enable FIFO
	*UART_FCR = 0x3;
}

int uart_putchar(int c) {
	while((*UART_LSR & UART_LCR_THRE) == 0) {
		asm volatile("nop");
	}
	*UART_THR = (u8)c;

	return (u8)c;
}

int uart_puts(char *str) {
	while(*str) {
		uart_putchar(*str++);
	}
	return 0;
}

int uart_getc(void) {
	while(!(*UART_LSR & 0x1)) {
		asm volatile("nop");
	}
	return *UART_RBR;
}
