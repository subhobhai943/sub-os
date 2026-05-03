/* SUB OS - AArch64 GIC-400 driver header */
#ifndef AARCH64_GIC_H
#define AARCH64_GIC_H

void         gic_init(void);
void         gic_enable_irq(unsigned int irq);
void         gic_disable_irq(unsigned int irq);
unsigned int gic_ack(void);
void         gic_eoi(unsigned int irq_id);

/* Barrier helper (gic_asm.S) */
void gic_dsb_sy(void);

#endif /* AARCH64_GIC_H */
