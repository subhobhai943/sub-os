/* SUB OS - AArch64 GIC-400 driver header */
#ifndef AARCH64_GIC_H
#define AARCH64_GIC_H

void         gic_init(void);
void         gic_enable_irq(unsigned int irq);
void         gic_disable_irq(unsigned int irq);
unsigned int gic_ack(void);
void         gic_eoi(unsigned int irq_id);

/* ASM helpers */
void gic_enable_irq_asm(unsigned int irq);
void gic_disable_irq_asm(unsigned int irq);
void gic_eoi_asm(unsigned int irq_id);

#endif /* AARCH64_GIC_H */
