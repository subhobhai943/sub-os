/* SUB OS - AArch64 GIC-400 Interrupt Controller Driver
 * Targets QEMU virt: GICD @ 0x08000000, GICC @ 0x08010000
 * All register access is pure C MMIO — no duplicate ASM stubs.
 */

#include "../include/gic.h"

#define GICD_BASE   0x08000000UL
#define GICC_BASE   0x08010000UL

/* GICD registers */
#define GICD_CTLR        (GICD_BASE + 0x000)
#define GICD_TYPER       (GICD_BASE + 0x004)
#define GICD_IGROUPR0    (GICD_BASE + 0x080)
#define GICD_ISENABLER0  (GICD_BASE + 0x100)
#define GICD_ICENABLER0  (GICD_BASE + 0x180)
#define GICD_ICPENDR0    (GICD_BASE + 0x280)
#define GICD_IPRIORITYR0 (GICD_BASE + 0x400)
#define GICD_ITARGETSR0  (GICD_BASE + 0x800)

/* GICC registers */
#define GICC_CTLR  (GICC_BASE + 0x000)
#define GICC_PMR   (GICC_BASE + 0x004)
#define GICC_BPR   (GICC_BASE + 0x008)
#define GICC_IAR   (GICC_BASE + 0x00C)
#define GICC_EOIR  (GICC_BASE + 0x010)

static inline void wr32(unsigned long a, unsigned int v) {
    *(volatile unsigned int *)a = v;
}
static inline unsigned int rd32(unsigned long a) {
    return *(volatile unsigned int *)a;
}

void gic_init(void) {
    unsigned int typer, it_lines, i;

    /* Disable distributor */
    wr32(GICD_CTLR, 0);

    typer    = rd32(GICD_TYPER);
    it_lines = (typer & 0x1F) + 1;

    /* Disable all, clear pending, set Group 0 */
    for (i = 0; i < it_lines; i++) {
        wr32(GICD_IGROUPR0   + i * 4, 0x00000000);
        wr32(GICD_ICENABLER0 + i * 4, 0xFFFFFFFF);
        wr32(GICD_ICPENDR0   + i * 4, 0xFFFFFFFF);
    }
    /* Priority 0xA0 for all */
    for (i = 0; i < it_lines * 8; i++)
        wr32(GICD_IPRIORITYR0 + i * 4, 0xA0A0A0A0);
    /* Target all SPIs to CPU0 */
    for (i = 8; i < it_lines * 8; i++)
        wr32(GICD_ITARGETSR0 + i * 4, 0x01010101);

    /* Enable distributor */
    wr32(GICD_CTLR, 1);

    /* CPU interface: allow all priorities, no preemption */
    wr32(GICC_PMR,  0xFF);
    wr32(GICC_BPR,  0x00);
    wr32(GICC_CTLR, 1);
}

void gic_enable_irq(unsigned int irq) {
    wr32(GICD_ISENABLER0 + (irq / 32) * 4, 1u << (irq % 32));
}

void gic_disable_irq(unsigned int irq) {
    wr32(GICD_ICENABLER0 + (irq / 32) * 4, 1u << (irq % 32));
}

unsigned int gic_ack(void) {
    return rd32(GICC_IAR) & 0x3FFu;
}

void gic_eoi(unsigned int irq_id) {
    wr32(GICC_EOIR, irq_id);
}
