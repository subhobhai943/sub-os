[bits 32]
; TSS Helper Functions
; SUB OS - Task State Segment

global tss_flush
global gdt_set_tss

tss_flush:
    mov ax, 0x28
    ltr ax
    ret

gdt_set_tss:
    ret
