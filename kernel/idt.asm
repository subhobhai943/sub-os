[bits 32]
; Interrupt Descriptor Table
; SUB OS - Interrupt handling

section .data
    idt_start:
        times 256 * 8 db 0  ; 256 entries, 8 bytes each
    idt_end:

    idt_descriptor:
        dw idt_end - idt_start - 1  ; IDT size
        dd idt_start                 ; IDT address

section .text
    global idt_load
    global idt_set_gate

; Load IDT
idt_load:
    lidt [idt_descriptor]
    ret

; Set IDT gate
; Parameters: vector (stack+4), base (stack+8), selector (stack+12), flags (stack+16)
idt_set_gate:
    push ebp
    mov ebp, esp
    push ebx
    
    mov ebx, [ebp + 8]   ; vector number
    shl ebx, 3           ; multiply by 8 (gate size)
    add ebx, idt_start   ; ebx now points to the IDT entry
    
    mov eax, [ebp + 12]  ; base address
    mov [ebx], ax        ; lower 16 bits of base
    shr eax, 16
    mov [ebx + 6], ax    ; upper 16 bits of base
    
    mov ax, [ebp + 16]   ; selector
    mov [ebx + 2], ax
    
    mov al, [ebp + 20]   ; flags
    mov [ebx + 5], al
    
    mov byte [ebx + 4], 0  ; reserved, must be zero
    
    pop ebx
    mov esp, ebp
    pop ebp
    ret
