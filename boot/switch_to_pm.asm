[bits 16]
; Switch to 32-bit Protected Mode
; SUB OS Boot - Protected Mode Entry

KERNEL_OFFSET equ 0x1000   ; Where we loaded the kernel

switch_to_pm:
    cli                     ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load GDT (defined in gdt.asm, included by boot.asm)
    mov eax, cr0
    or  eax, 0x1            ; Set PE bit
    mov cr0, eax
    jmp 0x08:init_pm        ; Far jump to flush pipeline / load CS

[bits 32]
init_pm:
    ; Reload all data segment registers with flat data selector
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up 32-bit stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Jump to kernel entry point
    jmp KERNEL_OFFSET
