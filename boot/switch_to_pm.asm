[bits 16]
global gdt_descriptor
extern gdt_descriptor
; Switch to 32-bit Protected Mode

switch_to_pm:
    cli                        ; Disable interrupts
    lgdt [gdt_descriptor]      ; Load GDT
    mov eax, cr0
    or eax, 0x1                ; Set PE (Protection Enable) bit
    mov cr0, eax
    jmp 0x08:init_pm           ; Far jump to flush pipeline (use selector value directly)

[bits 32]
init_pm:
    mov ax, 0x10               ; Update segment registers (use selector value directly)
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000           ; Update stack position
    mov esp, ebp
    call BEGIN_PM              ; Call protected mode entry point
BEGIN_PM:
    ret
