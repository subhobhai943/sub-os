/* SUB OS - AArch64 Kernel Main
 * This is the C entry point after assembly start-up in start.S.
 * Initialises architecture-specific drivers and starts the kernel loop.
 */

#include "../include/uart.h"
#include "../include/gic.h"
#include "../include/timer.h"
#include "../include/mmu.h"

/* Defined in vectors.S */
extern void exception_vectors(void);

static void install_vectors(void) {
    asm volatile("msr vbar_el1, %0; isb" :: "r"((unsigned long)exception_vectors));
}

void main(void) {
    /* 1. Bring up UART first so we can print debug info */
    uart_init();
    uart_puts("SUB OS - AArch64 port starting...\n");

    /* 2. Install exception/interrupt vector table */
    install_vectors();
    uart_puts("[OK] Exception vectors installed\n");

    /* 3. Initialise GIC interrupt controller */
    gic_init();
    uart_puts("[OK] GIC-400 initialised\n");

    /* 4. Initialise MMU (identity map first 1 GB) */
    mmu_init();
    uart_puts("[OK] MMU enabled (identity map 0-1 GB)\n");

    /* 5. Initialise system timer (100 Hz) */
    timer_init();
    uart_puts("[OK] Generic timer initialised at 100 Hz\n");

    uart_puts("SUB OS AArch64 boot complete. Entering idle loop.\n");

    /* Idle loop: WFI until next interrupt */
    for (;;) {
        asm volatile("wfi");
    }
}
