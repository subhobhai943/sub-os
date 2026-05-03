/* SUB OS - AArch64 Generic Timer Driver
 * Uses the AArch64 virtual or physical timer (EL1 physical timer here).
 * Connects to GIC SPI 30 (PPI IRQ 30 in QEMU virt).
 */

#include "../include/timer.h"
#include "../include/gic.h"

#define TIMER_IRQ   30      /* EL1 physical timer PPI */
#define TICKS_PER_SEC 100   /* 100 Hz = 10 ms tick */

static volatile unsigned long timer_ticks = 0;
static unsigned long timer_freq = 0;

void timer_init(void) {
    /* Read counter frequency from CNTFRQ_EL0 */
    asm volatile("mrs %0, cntfrq_el0" : "=r"(timer_freq));

    /* Set comparison value: freq / TICKS_PER_SEC ticks from now */
    unsigned long tval = timer_freq / TICKS_PER_SEC;
    asm volatile("msr cntp_tval_el0, %0" :: "r"(tval));

    /* Enable EL1 physical timer, unmask interrupt */
    asm volatile("msr cntp_ctl_el0, %0" :: "r"((unsigned long)1));

    /* Enable the GIC line for this PPI */
    gic_enable_irq(TIMER_IRQ);

    /* Unmask IRQs at EL1 */
    asm volatile("msr daifclr, #2");
}

void timer_handler(void) {
    timer_ticks++;

    /* Reload timer */
    unsigned long tval = timer_freq / TICKS_PER_SEC;
    asm volatile("msr cntp_tval_el0, %0" :: "r"(tval));
}

unsigned long timer_get_ticks(void) {
    return timer_ticks;
}

void timer_delay_ms(unsigned int ms) {
    unsigned long start = timer_ticks;
    unsigned long target = start + (ms * TICKS_PER_SEC) / 1000;
    while (timer_ticks < target)
        asm volatile("wfe");
}
