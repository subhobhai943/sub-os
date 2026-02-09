// Simple PL011 UART Driver for QEMU Virt
volatile unsigned int * const UART0DR = (unsigned int *)0x09000000;
volatile unsigned int * const UART0FR = (unsigned int *)0x09000018;

void uart_putc(char c) {
    *UART0DR = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

char uart_getc() {
    // Check RXFE (Receive FIFO Empty) - bit 4
    if (*UART0FR & 0x10) {
        return 0;
    }
    return (char)(*UART0DR);
}
