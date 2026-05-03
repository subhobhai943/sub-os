/* SUB OS - AArch64 Kernel Main */

#include "../include/uart.h"
#include "../include/gic.h"
#include "../include/timer.h"
#include "../include/desktop.h"

extern void exception_vectors(void);

static void install_vectors(void) {
    __asm__ volatile(
        "msr vbar_el1, %0\n\t"
        "isb"
        :: "r"((unsigned long)exception_vectors)
    );
}

void main(void) {
    uart_init();
    uart_puts("SUB OS - AArch64 booting...\r\n");

    install_vectors();
    uart_puts("[OK] Vectors\r\n");

    gic_init();
    uart_puts("[OK] GIC\r\n");

    timer_init();
    uart_puts("[OK] Timer\r\n");

    uart_puts("[..] Starting GUI...\r\n");
    gui_main();

    /* Should never reach here */
    for (;;) __asm__ volatile("wfi");
}
