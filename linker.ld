/* SUB OS Linker Script */

ENTRY(kernel_main)

SECTIONS
{
    . = 0x1000; /* Kernel loaded at 0x1000 by bootloader */

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }

    kernel_end = .;
}
