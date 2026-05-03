/* SUB OS - AArch64 GIC low-level cache/barrier helpers
 * NOTE: All GIC enable/disable/EOI logic is in gic.c (C MMIO).
 * This file only contains barriers needed before/after MMIO sequences.
 */

.section ".text"
.global gic_dsb_sy

/* void gic_dsb_sy(void) -- full system data-sync barrier */
gic_dsb_sy:
    dsb     sy
    isb
    ret
