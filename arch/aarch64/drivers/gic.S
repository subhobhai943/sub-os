/* SUB OS - AArch64 GIC-400 low-level helpers
 * Used for enabling/disabling specific SPI/PPI/SGI lines.
 */

.section ".text"
.global gic_enable_irq_asm
.global gic_disable_irq_asm
.global gic_eoi_asm

/* void gic_enable_irq_asm(uint32_t irq) -- x0 = irq number */
gic_enable_irq_asm:
    /* GICD_ISENABLERn: base 0x08000100, word offset = irq/32 */
    mov     x1, #0x08000000     /* GICD base (QEMU virt) */
    add     x1, x1, #0x0100     /* GICD_ISENABLER0 */
    lsr     x2, x0, #5          /* word index */
    lsl     x2, x2, #2          /* byte offset */
    add     x1, x1, x2
    and     x0, x0, #31         /* bit within word */
    mov     x3, #1
    lsl     x3, x3, x0
    str     w3, [x1]
    ret

/* void gic_disable_irq_asm(uint32_t irq) */
gic_disable_irq_asm:
    mov     x1, #0x08000000
    add     x1, x1, #0x0180     /* GICD_ICENABLER0 */
    lsr     x2, x0, #5
    lsl     x2, x2, #2
    add     x1, x1, x2
    and     x0, x0, #31
    mov     x3, #1
    lsl     x3, x3, x0
    str     w3, [x1]
    ret

/* void gic_eoi_asm(uint32_t irqid) -- GICC_EOIR */
gic_eoi_asm:
    mov     x1, #0x08010000     /* GICC base */
    add     x1, x1, #0x0010     /* GICC_EOIR */
    str     w0, [x1]
    ret
