/* SUB OS - AArch64 Kernel Main */

#include "../include/uart.h"
#include "../include/gic.h"
#include "../include/timer.h"
#include "../include/shell.h"

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

    install_vectors();
    gic_init();
    timer_init();

    /* Hand off to interactive shell (never returns) */
    shell_run();
}
