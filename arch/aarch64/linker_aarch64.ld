/* SUB OS - AArch64 Linker Script
 * Load address: 0x40080000 (QEMU virt default for non-secure EL1 kernel)
 */

ENTRY(_start)

SECTIONS
{
    . = 0x40080000;

    .text.boot : {
        KEEP(*(.text.boot))
    }

    .text.vectors : {
        . = ALIGN(2048);    /* Exception vectors need 2 KB alignment */
        KEEP(*(.text.vectors))
    }

    .text : {
        *(.text .text.*)
    }

    .rodata : ALIGN(4096) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4096) {
        *(.data .data.*)
    }

    .bss : ALIGN(4096) {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* 16 KB stack (defined in start.S, not in BSS) */
    . = ALIGN(4096);

    kernel_end = .;
}
