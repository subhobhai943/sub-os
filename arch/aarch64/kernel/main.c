/* SUB OS - AArch64 Kernel Main */

#include "../include/uart.h"
#include "../include/gic.h"
#include "../include/timer.h"
#include "../include/mmu.h"

extern void exception_vectors(void);

static void install_vectors(void) {
    __asm__ volatile("msr vbar_el1, %0\n\t"
                     "isb" :: "r"((unsigned long)exception_vectors));
}

void main(void) {
    uart_init();
    uart_puts("SUB OS - AArch64 port starting...\n");

    install_vectors();
    uart_puts("[OK] Exception vectors installed\n");

    gic_init();
    uart_puts("[OK] GIC-400 initialised\n");

    /* MMU init AFTER GIC so UART is already working for debug */
    uart_puts("[..] Enabling MMU...\n");
    mmu_init();
    uart_puts("[OK] MMU enabled\n");

    timer_init();
    uart_puts("[OK] Generic timer initialised at 100 Hz\n");

    uart_puts("SUB OS AArch64 boot complete. Entering idle loop.\n");

    for (;;) {
        __asm__ volatile("wfi");
    }
}
