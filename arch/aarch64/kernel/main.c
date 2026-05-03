/* SUB OS - AArch64 Kernel Main */

#include "../include/uart.h"
#include "../include/gic.h"
#include "../include/timer.h"

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
    uart_puts("SUB OS - AArch64 port starting...\n");

    install_vectors();
    uart_puts("[OK] Exception vectors installed\n");

    gic_init();
    uart_puts("[OK] GIC-400 initialised\n");

    /* MMU skipped: raw binary loaded by QEMU -kernel has no guaranteed
     * PA for static page table arrays. Re-enable once an ELF loader
     * or identity-map stub is in place. */
    uart_puts("[--] MMU skipped (raw binary mode)\n");

    timer_init();
    uart_puts("[OK] Generic timer initialised at 100 Hz\n");

    uart_puts("SUB OS AArch64 boot complete. Entering idle loop.\n");
    uart_puts("     Press Ctrl-A then X to exit QEMU.\n");

    for (;;) {
        __asm__ volatile("wfi");
    }
}
