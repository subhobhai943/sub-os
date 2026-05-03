/* SUB OS - AArch64 PL011 UART Driver
 * Targets QEMU virt machine PL011 at 0x09000000.
 * Replaces VGA/keyboard I/O from the x86 build.
 */

#include "../include/uart.h"

#define UART0_BASE  0x09000000UL

/* PL011 register offsets */
#define UARTDR      0x000   /* Data Register */
#define UARTFR      0x018   /* Flag Register */
#define UARTIBRD    0x024   /* Integer Baud Rate */
#define UARTFBRD    0x028   /* Fractional Baud Rate */
#define UARTLCR_H   0x02C   /* Line Control */
#define UARTCR      0x030   /* Control Register */
#define UARTIMSC    0x038   /* Interrupt Mask */
#define UARTICR     0x044   /* Interrupt Clear */

/* Flag Register bits */
#define FR_TXFF     (1 << 5)  /* TX FIFO full */
#define FR_RXFE     (1 << 4)  /* RX FIFO empty */
#define FR_BUSY     (1 << 3)  /* UART busy */

/* UARTLCR_H bits */
#define LCR_FEN     (1 << 4)  /* FIFO enable */
#define LCR_8N1     (0x3 << 5) /* 8 data bits */

/* UARTCR bits */
#define CR_RXE      (1 << 9)
#define CR_TXE      (1 << 8)
#define CR_UARTEN   (1 << 0)

static inline void mmio_write(unsigned long addr, unsigned int val) {
    *(volatile unsigned int *)addr = val;
}
static inline unsigned int mmio_read(unsigned long addr) {
    return *(volatile unsigned int *)addr;
}

void uart_init(void) {
    /* Disable UART */
    mmio_write(UART0_BASE + UARTCR, 0);

    /* Wait for UART to finish transmitting */
    while (mmio_read(UART0_BASE + UARTFR) & FR_BUSY);

    /* Set baud rate: 115200 @ 24 MHz (QEMU default)
     * Divisor = 24000000 / (16 * 115200) = 13.020...
     * IBRD = 13, FBRD = round(0.020 * 64) = 1 */
    mmio_write(UART0_BASE + UARTIBRD, 13);
    mmio_write(UART0_BASE + UARTFBRD, 1);

    /* 8N1, FIFO enable */
    mmio_write(UART0_BASE + UARTLCR_H, LCR_FEN | LCR_8N1);

    /* Clear interrupts */
    mmio_write(UART0_BASE + UARTICR, 0x7FF);

    /* Enable TX, RX, UART */
    mmio_write(UART0_BASE + UARTCR, CR_UARTEN | CR_TXE | CR_RXE);
}

void uart_putc(char c) {
    /* Wait while TX FIFO is full */
    while (mmio_read(UART0_BASE + UARTFR) & FR_TXFF);
    mmio_write(UART0_BASE + UARTDR, (unsigned int)c);
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}

char uart_getc(void) {
    /* Wait while RX FIFO is empty */
    while (mmio_read(UART0_BASE + UARTFR) & FR_RXFE);
    return (char)(mmio_read(UART0_BASE + UARTDR) & 0xFF);
}

int uart_getc_nonblock(char *c) {
    if (mmio_read(UART0_BASE + UARTFR) & FR_RXFE) return 0;
    *c = (char)(mmio_read(UART0_BASE + UARTDR) & 0xFF);
    return 1;
}
