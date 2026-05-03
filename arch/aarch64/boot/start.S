/* SUB OS - AArch64 Kernel Entry Point
 * Entered from U-Boot / QEMU virt machine at EL2 or EL1.
 * We drop to EL1 if needed, disable MMU/caches, clear BSS, then call main().
 */

.section ".text.boot"
.global _start

_start:
    /* -------------------------------------------------------
     * 1. Determine current Exception Level
     *    If EL2, configure HCR_EL2 and drop to EL1
     * ------------------------------------------------------- */
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    cmp     x0, #2
    b.ne    1f              /* already EL1, skip drop */

    /* Configure HCR_EL2: AArch64 NS EL1 */
    mov     x0, #(1 << 31)  /* RW = 1: EL1 is AArch64 */
    orr     x0, x0, #(1 << 1) /* SWIO */
    msr     hcr_el2, x0

    /* SCTLR_EL1: clear M, C, I (MMU/cache off) */
    msr     sctlr_el1, xzr

    /* Set ELR_EL2 = address of el1_entry, SPSR_EL2 for EL1h */
    adr     x0, el1_entry
    msr     elr_el2, x0
    mov     x0, #0x3c5      /* EL1h, all IRQ/FIQ/SError masked */
    msr     spsr_el2, x0
    eret

1:  /* EL1 entry ------------------------------------------- */
el1_entry:
    /* Disable MMU, caches, branch prediction in SCTLR_EL1 */
    mrs     x0, sctlr_el1
    bic     x0, x0, #(1 << 0)   /* M  - MMU off */
    bic     x0, x0, #(1 << 2)   /* C  - D-cache off */
    bic     x0, x0, #(1 << 12)  /* I  - I-cache off */
    msr     sctlr_el1, x0
    isb

    /* Invalidate TLBs and caches */
    tlbi    vmalle1
    ic      iallu
    dsb     sy
    isb

    /* Set stack pointer (grows down from __stack_top) */
    adr     x0, __stack_top
    mov     sp, x0

    /* -------------------------------------------------------
     * 2. Clear BSS
     * ------------------------------------------------------- */
    adr     x1, __bss_start
    adr     x2, __bss_end
    cmp     x1, x2
    b.ge    2f
bss_loop:
    str     xzr, [x1], #8
    cmp     x1, x2
    b.lt    bss_loop
2:

    /* -------------------------------------------------------
     * 3. Call kernel main()
     * ------------------------------------------------------- */
    bl      main

    /* Should never return; halt all CPUs */
halt:
    wfe
    b       halt

.section ".data"
.align 12   /* 4 KB stack */
.space 0x4000  /* 16 KB initial stack */
__stack_top:
