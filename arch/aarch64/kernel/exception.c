/* SUB OS - AArch64 Exception Handlers
 * Called from the vector table stubs in vectors.S.
 */

#include "../include/uart.h"
#include "../include/gic.h"
#include "../include/timer.h"

/* Minimal saved register frame passed by vectors.S */
typedef struct {
    unsigned long x[31];
    unsigned long pc;
    unsigned long pstate;
} cpu_regs_t;

void handle_sync(cpu_regs_t *regs) {
    unsigned long esr, far;
    asm volatile("mrs %0, esr_el1"  : "=r"(esr));
    asm volatile("mrs %0, far_el1"  : "=r"(far));

    uart_puts("[SYNC EXCEPTION]\n");
    (void)regs; (void)esr; (void)far;
    /* TODO: decode EC field and forward to appropriate handler */
    while (1) asm volatile("wfe");
}

void handle_irq(cpu_regs_t *regs) {
    unsigned int irq_id = gic_ack();
    (void)regs;

    switch (irq_id) {
    case 30:                /* EL1 physical timer PPI */
        timer_handler();
        break;
    default:
        /* Spurious or unhandled IRQ */
        uart_puts("[IRQ] unhandled\n");
        break;
    }

    gic_eoi(irq_id);
}

void handle_fiq(cpu_regs_t *regs) {
    (void)regs;
    uart_puts("[FIQ] unhandled\n");
    while (1) asm volatile("wfe");
}
