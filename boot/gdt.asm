[bits 16]
; Global Descriptor Table
; SUB OS - GDT for Protected Mode switch
; This file is %included by boot.asm (flat binary), not linked as ELF.

gdt_start:

gdt_null:               ; Mandatory null descriptor
    dd 0x0
    dd 0x0

gdt_code:               ; Kernel Code  (selector 0x08)
    dw 0xffff           ; Limit 0-15
    dw 0x0000           ; Base  0-15
    db 0x00             ; Base  16-23
    db 10011010b        ; Access: present, ring0, code, readable
    db 11001111b        ; Flags: 4KB gran, 32-bit + limit 16-19
    db 0x00             ; Base  24-31

gdt_data:               ; Kernel Data  (selector 0x10)
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b        ; Access: present, ring0, data, writable
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size (limit)
    dd gdt_start                 ; GDT linear address
