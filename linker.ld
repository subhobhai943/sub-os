/* SUB OS Linker Script */

ENTRY(main)

SECTIONS
{
    . = 0x1000; /* Kernel starts at 0x1000 */
    
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
        *(.bss)
    }
}
