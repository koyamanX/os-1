#ifndef UART_H
#define UART_H

void uart_init(void);
int uart_putchar(int c);
int uart_puts(char *str);
int uart_getc(void);
void uart_intr(void);

#define UART_BASE 0x10000000
#define UART_RBR ((volatile u8 *)UART_BASE + 0x0)
#define UART_THR ((volatile u8 *)UART_BASE + 0x0)
#define UART_DLL ((volatile u8 *)UART_BASE + 0x0)
#define UART_DLM ((volatile u8 *)UART_BASE + 0x1)
#define UART_IER ((volatile u8 *)UART_BASE + 0x1)
#define UART_IIR ((volatile u8 *)UART_BASE + 0x2)
#define UART_FCR ((volatile u8 *)UART_BASE + 0x2)
#define UART_LCR ((volatile u8 *)UART_BASE + 0x3)
#define UART_MCR ((volatile u8 *)UART_BASE + 0x4)
#define UART_LSR ((volatile u8 *)UART_BASE + 0x5)
#define UART_MSR ((volatile u8 *)UART_BASE + 0x6)
#define UART_SCR ((volatile u8 *)UART_BASE + 0x7)

#define UART_LCR_DLAB (1 << 7)
#define UART_LCR_THRE (1 << 5)

#endif
