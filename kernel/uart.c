// Simple PL011 UART Driver for QEMU Virt
volatile unsigned int * const UART0DR = (unsigned int *)0x09000000;

void uart_putc(char c) {
    *UART0DR = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}
