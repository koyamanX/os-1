#include <printk.h>
#include <riscv.h>
#include <sleeplock.h>
#include <uart.h>

#define N 1024
struct ring_buffer {
    char buffer[N];
    int head;
    int tail;
    int count;
    struct sleeplock lk;
} rxbuf, txbuf;

void uart_init(void) {
    // Enable interrupt
    *UART_IER = 1;
    // 38.4 K baud rate
    *UART_LCR = UART_LCR_DLAB;
    *UART_DLL = 0x3;
    *UART_DLM = 0x0;

    // 8 bits char
    *UART_LCR = 0x3;
    // clear and enable FIFO
    *UART_FCR = 0x3;

    rxbuf.head = 0;
    rxbuf.tail = 0;
    rxbuf.count = 0;
    sleep_lock_init(&rxbuf.lk, "");
    txbuf.head = 0;
    txbuf.tail = 0;
    txbuf.count = 0;
    sleep_lock_init(&txbuf.lk, "");
}

int uart_putchar(int c) {
    if ((*UART_LSR & UART_LCR_THRE)) {
        *UART_THR = (u8)c;
        return (u8)c;
    }
    if (txbuf.count >= N) {
        sleep_lock(&txbuf.lk);
    }
    txbuf.buffer[txbuf.tail] = c;
    txbuf.tail = (txbuf.tail + 1) % N;
    txbuf.count++;

    return c;
}

int uart_puts(char *str) {
    while (*str) {
        uart_putchar(*str++);
    }
    return 0;
}

int uart_getc(void) {
    while (rxbuf.count == 0) {
        sleep_lock(&rxbuf.lk);
    }
    char c = rxbuf.buffer[rxbuf.head];
    rxbuf.head = (rxbuf.head + 1) % N;
    rxbuf.count--;
    return c;
}

void uart_intr(void) {
    int newline = 0;

    if (rxbuf.count >= N) {
        printk("Ring buffer overflow\n");
    }

    while (1) {
        if ((*UART_LSR & 0x1)) {
            int c = *UART_RBR;

            if (c == '\r' || c == '\n') {
                newline = 1;
                c = '\n';
            }
            if (c == 0x7f) {
                if (rxbuf.count == 0)
                    continue;
                uart_putchar('\b');
                uart_putchar(' ');
                uart_putchar('\b');
                rxbuf.tail = (rxbuf.tail - 1) % N;
                rxbuf.count--;
                continue;
            }
            uart_putchar(c);
            rxbuf.buffer[rxbuf.tail] = c;
            rxbuf.tail = (rxbuf.tail + 1) % N;
            rxbuf.count++;
            if (newline)
                break;
        } else {
            break;
        }
    }
    if (newline) {
        sleep_unlock(&rxbuf.lk);
    }

    if (!txbuf.count && (*UART_LSR & UART_LCR_THRE)) {
        sleep_unlock(&txbuf.lk);
    }

    while (txbuf.count) {
        if ((*UART_LSR & UART_LCR_THRE)) {
            char c = txbuf.buffer[txbuf.head];
            txbuf.head = (txbuf.head + 1) % N;
            txbuf.count--;
            *UART_THR = (u8)c;
        } else {
            return;
        }
    }
}
