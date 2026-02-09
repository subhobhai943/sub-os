/* SUB OS Linker Script */

ENTRY(_start)

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
    
    /* Mark end of kernel */
    kernel_end = .;
}
