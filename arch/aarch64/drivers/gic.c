/* SUB OS - AArch64 GIC-400 Interrupt Controller Driver
 * Targets QEMU virt: GICD @ 0x08000000, GICC @ 0x08010000
 */

#include "../include/gic.h"

#define GICD_BASE   0x08000000UL
#define GICC_BASE   0x08010000UL

/* GICD registers */
#define GICD_CTLR       (GICD_BASE + 0x000)
#define GICD_TYPER      (GICD_BASE + 0x004)
#define GICD_IGROUPR0   (GICD_BASE + 0x080)
#define GICD_ISENABLER0 (GICD_BASE + 0x100)
#define GICD_ICENABLER0 (GICD_BASE + 0x180)
#define GICD_ICPENDR0   (GICD_BASE + 0x280)
#define GICD_IPRIORITYR0 (GICD_BASE + 0x400)
#define GICD_ITARGETSR0  (GICD_BASE + 0x800)
#define GICD_ICFGR0      (GICD_BASE + 0xC00)

/* GICC registers */
#define GICC_CTLR   (GICC_BASE + 0x000)
#define GICC_PMR    (GICC_BASE + 0x004)
#define GICC_BPR    (GICC_BASE + 0x008)
#define GICC_IAR    (GICC_BASE + 0x00C)
#define GICC_EOIR   (GICC_BASE + 0x010)

static inline void mmio_write32(unsigned long a, unsigned int v) {
    *(volatile unsigned int *)a = v;
}
static inline unsigned int mmio_read32(unsigned long a) {
    return *(volatile unsigned int *)a;
}

void gic_init(void) {
    unsigned int typer, it_lines;
    unsigned int i;

    /* Disable distributor */
    mmio_write32(GICD_CTLR, 0);

    typer = mmio_read32(GICD_TYPER);
    it_lines = (typer & 0x1F) + 1;  /* Number of 32-IRQ groups */

    /* Set all interrupts: Group 0, disable, clear pending, priority 0xA0 */
    for (i = 0; i < it_lines; i++) {
        mmio_write32(GICD_IGROUPR0  + i * 4, 0x00000000);
        mmio_write32(GICD_ICENABLER0 + i * 4, 0xFFFFFFFF);
        mmio_write32(GICD_ICPENDR0  + i * 4, 0xFFFFFFFF);
    }
    /* Set priority 0xA0 for all */
    for (i = 0; i < it_lines * 8; i++)
        mmio_write32(GICD_IPRIORITYR0 + i * 4, 0xA0A0A0A0);
    /* Target CPU0 for all SPIs */
    for (i = 8; i < it_lines * 8; i++)
        mmio_write32(GICD_ITARGETSR0 + i * 4, 0x01010101);

    /* Enable distributor */
    mmio_write32(GICD_CTLR, 1);

    /* CPU interface: priority mask = 0xFF (all), no preemption */
    mmio_write32(GICC_PMR, 0xFF);
    mmio_write32(GICC_BPR, 0x00);
    /* Enable CPU interface */
    mmio_write32(GICC_CTLR, 1);
}

void gic_enable_irq(unsigned int irq) {
    unsigned int reg = GICD_ISENABLER0 + (irq / 32) * 4;
    mmio_write32(reg, 1u << (irq % 32));
}

void gic_disable_irq(unsigned int irq) {
    unsigned int reg = GICD_ICENABLER0 + (irq / 32) * 4;
    mmio_write32(reg, 1u << (irq % 32));
}

unsigned int gic_ack(void) {
    return mmio_read32(GICC_IAR) & 0x3FF;
}

void gic_eoi(unsigned int irq_id) {
    mmio_write32(GICC_EOIR, irq_id);
}
