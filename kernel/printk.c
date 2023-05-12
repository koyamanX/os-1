#include <printk.h>
#include <riscv.h>
#include <stdarg.h>
#include <uart.h>

char *ulltoa(u64 n, char *buffer, int radix) {
    char *p;
    int c = 0;

    p = buffer;
    if (n == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return p;
    }
    while (n > 0) {
        *buffer = "0123456789abcdef"[(n % radix)];
        buffer++;
        n = n / radix;
        c++;
    }
    *buffer = '\0';
    c--;
    for (int i = 0, j = c; i <= c / 2; i++, j--) {
        int x;
        x = p[i];
        p[i] = p[j];
        p[j] = x;
    }
    return p;
}

int printk(const char *format, ...) {
    va_list ap;
    const char *bp;
    char buf[256];

    va_start(ap, format);

    bp = format;
    while (*bp) {
        if (*bp == '%') {
            bp++;
            switch (*bp) {
                case 's':
                    uart_puts(va_arg(ap, char *));
                    bp++;
                    break;
                case 'x':
                case 'p':
                    ulltoa(va_arg(ap, u64), buf, 16);
                    uart_puts(buf);
                    bp++;
                    break;
                case 'c':
                    uart_putchar(*bp);
                    bp++;
                    break;
            }
        } else {
            uart_putchar(*bp);
            bp++;
        }
    }

    va_end(ap);
    return 0;
}
