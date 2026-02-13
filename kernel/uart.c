#include "uart.h"
#include "kernel.h"

#if defined(__aarch64__) || defined(__arm__)

// Simple PL011 UART Driver for QEMU Virt
volatile unsigned int * const UART0DR = (unsigned int *)0x09000000;
volatile unsigned int * const UART0FR = (unsigned int *)0x09000018;
volatile unsigned int * const UART0IBRD = (unsigned int *)0x09000024;
volatile unsigned int * const UART0FBRD = (unsigned int *)0x09000028;
volatile unsigned int * const UART0LCR_H = (unsigned int *)0x0900002C;
volatile unsigned int * const UART0CR = (unsigned int *)0x09000030;
volatile unsigned int * const UART0IMSC = (unsigned int *)0x09000038;

void uart_init() {
    // Disable UART
    *UART0CR = 0;

    // Mask all interrupts
    *UART0IMSC = 0;

    // Set baud rate (Assuming 24MHz clock, 38400 baud)
    // Divider = 24000000 / (16 * 38400) = 39.0625
    // IBRD = 39
    // FBRD = 0.0625 * 64 + 0.5 = 4.5 -> 4
    *UART0IBRD = 39;
    *UART0FBRD = 4;

    // 8 bits, no parity, 1 stop bit, FIFO enabled
    *UART0LCR_H = (1 << 4) | (3 << 5); 

    // Enable UART, TX, RX
    *UART0CR = (1 << 0) | (1 << 8) | (1 << 9);
}

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

#else

// x86 UART Driver (COM1)
#define COM1 0x3F8

void uart_init() {
    outb(COM1 + 1, 0x00);    // Disable all interrupts
    outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1 + 1, 0x00);    //                  (hi byte)
    outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int uart_received() {
    return inb(COM1 + 5) & 1;
}

char uart_getc() {
    if (uart_received() == 0) return 0;
    return inb(COM1);
}

int uart_is_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

void uart_putc(char c) {
    while (uart_is_transmit_empty() == 0);
    outb(COM1, c);
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

#endif
