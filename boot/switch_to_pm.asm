[bits 16]
global BEGIN_PM
; Switch to 32-bit Protected Mode

switch_to_pm:
    cli                        ; Disable interrupts
    lgdt [gdt_descriptor]      ; Load GDT
    
    mov eax, cr0
    or eax, 0x1                ; Set PE (Protection Enable) bit
    mov cr0, eax
    
    jmp CODE_SEG:init_pm       ; Far jump to flush pipeline

[bits 32]
init_pm:
    mov ax, DATA_SEG           ; Update segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, 0x90000           ; Update stack position
    mov esp, ebp
    
    call BEGIN_PM              ; Call protected mode entry point

; Provide default BEGIN_PM label to avoid linkage errors
BEGIN_PM:
    ret
