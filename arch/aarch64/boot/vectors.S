/* SUB OS - AArch64 Exception Vector Table
 * Each slot is exactly 128 bytes (32 instructions max).
 * We keep it simple: save minimal context, call C handler, restore.
 */

.macro VENTRY label
    .align 7
    b \label
.endm

.section ".text.vectors"
.global exception_vectors
.align 11   /* 2 KB aligned */
exception_vectors:

    /* --- Current EL with SP0 --- */
    VENTRY  sync_el1t
    VENTRY  irq_el1t
    VENTRY  fiq_el1t
    VENTRY  serr_el1t

    /* --- Current EL with SPx --- */
    VENTRY  sync_el1h
    VENTRY  irq_el1h
    VENTRY  fiq_el1h
    VENTRY  serr_el1h

    /* --- Lower EL AArch64 --- */
    VENTRY  sync_el0_64
    VENTRY  irq_el0_64
    VENTRY  fiq_el0_64
    VENTRY  serr_el0_64

    /* --- Lower EL AArch32 (not supported) --- */
    VENTRY  unsupported_vector
    VENTRY  unsupported_vector
    VENTRY  unsupported_vector
    VENTRY  unsupported_vector

/* ---- Stubs (call into C handlers) ---- */
.macro SAVE_REGS
    sub     sp, sp, #272
    stp     x0, x1,   [sp, #0]
    stp     x2, x3,   [sp, #16]
    stp     x4, x5,   [sp, #32]
    stp     x6, x7,   [sp, #48]
    stp     x8, x9,   [sp, #64]
    stp     x10, x11, [sp, #80]
    stp     x12, x13, [sp, #96]
    stp     x14, x15, [sp, #112]
    stp     x16, x17, [sp, #128]
    stp     x18, x19, [sp, #144]
    stp     x20, x21, [sp, #160]
    stp     x22, x23, [sp, #176]
    stp     x24, x25, [sp, #192]
    stp     x26, x27, [sp, #208]
    stp     x28, x29, [sp, #224]
    str     x30,      [sp, #240]
    mrs     x0, elr_el1
    mrs     x1, spsr_el1
    stp     x0, x1,   [sp, #248]
.endm

.macro RESTORE_REGS
    ldp     x0, x1,   [sp, #248]
    msr     elr_el1, x0
    msr     spsr_el1, x1
    ldp     x0, x1,   [sp, #0]
    ldp     x2, x3,   [sp, #16]
    ldp     x4, x5,   [sp, #32]
    ldp     x6, x7,   [sp, #48]
    ldp     x8, x9,   [sp, #64]
    ldp     x10, x11, [sp, #80]
    ldp     x12, x13, [sp, #96]
    ldp     x14, x15, [sp, #112]
    ldp     x16, x17, [sp, #128]
    ldp     x18, x19, [sp, #144]
    ldp     x20, x21, [sp, #160]
    ldp     x22, x23, [sp, #176]
    ldp     x24, x25, [sp, #192]
    ldp     x26, x27, [sp, #208]
    ldp     x28, x29, [sp, #224]
    ldr     x30,      [sp, #240]
    add     sp, sp, #272
.endm

sync_el1t:
sync_el1h:
sync_el0_64:
    SAVE_REGS
    mov     x0, sp
    bl      handle_sync
    RESTORE_REGS
    eret

irq_el1t:
irq_el1h:
irq_el0_64:
    SAVE_REGS
    mov     x0, sp
    bl      handle_irq
    RESTORE_REGS
    eret

fiq_el1t:
fiq_el1h:
fiq_el0_64:
    SAVE_REGS
    mov     x0, sp
    bl      handle_fiq
    RESTORE_REGS
    eret

serr_el1t:
serr_el1h:
serr_el0_64:
unsupported_vector:
    wfe
    b       unsupported_vector
