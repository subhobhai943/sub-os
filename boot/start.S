.global _start
.extern kernel_main

.section .text
_start:
    /* Setup stack */
    ldr sp, =stack_top

    /* Jump to C code */
    bl kernel_main

    /* Hang */
    b .

.section .bss
.align 16
stack_bottom:
.skip 4096 * 4
stack_top:
