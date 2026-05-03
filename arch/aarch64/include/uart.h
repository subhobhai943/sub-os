/* SUB OS - AArch64 UART driver header */
#ifndef AARCH64_UART_H
#define AARCH64_UART_H

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
char uart_getc(void);
int  uart_getc_nonblock(char *c);

#endif /* AARCH64_UART_H */
