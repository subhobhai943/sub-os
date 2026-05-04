[bits 32]
; TSS Helper Functions
; SUB OS - Task State Segment
;
; The GDT built by tss.c (via gdt_set_tss) writes descriptor slot 5
; (byte offset 0x28) into the GDT that was set up at boot.
; tss_flush loads that selector into the Task Register.

global tss_flush
global gdt_set_tss

; tss_flush - load TSS selector 0x28 into Task Register
; The | 3 makes it RPL=3 which LTR still accepts (privilege is in the
; descriptor type, not the selector RPL for system descriptors).
tss_flush:
    mov ax, 0x28
    ltr ax
    ret

; gdt_set_tss(unsigned int base, unsigned int limit)
;   Writes a TSS descriptor at GDT slot 5 (offset 0x28).
;   The GDT base address is hard-coded to 0x00000800 as set by gdt.asm.
;   Parameters on stack: [esp+4]=base, [esp+8]=limit
gdt_set_tss:
    push ebp
    mov  ebp, esp

    mov  edx, [ebp + 8]     ; base address of TSS
    mov  ecx, [ebp + 12]    ; limit

    ; GDT slot 5 = offset 40 bytes from GDT start
    ; GDT start address = gdt_start (assembled at link-time as flat address).
    ; For the boot GDT we stored gdt_descriptor at a known location;
    ; we read the GDT base from the GDTR to be safe.
    sub  esp, 6             ; make room for SGDT result
    sgdt [esp]              ; [esp+2] = GDT base (32-bit)
    mov  eax, [esp + 2]     ; eax = GDT base
    add  esp, 6

    add  eax, 40            ; slot 5 = 5 * 8 bytes

    ; Write a 32-bit TSS descriptor:
    ; Bytes 0-1: limit[0:15]
    mov  [eax + 0], cx      ; limit low 16 bits
    ; Bytes 2-3: base[0:15]
    mov  [eax + 2], dx      ; base  low 16 bits
    ; Byte  4  : base[16:23]
    shr  edx, 16
    mov  [eax + 4], dl      ; base mid byte
    ; Byte  5  : type=0x89 (present, 32-bit TSS available)
    mov  byte [eax + 5], 0x89
    ; Byte  6  : flags=0x00, limit[16:19]=0
    movzx ecx, cx
    shr  ecx, 16
    and  cl, 0x0F
    mov  [eax + 6], cl
    ; Byte  7  : base[24:31]
    mov  [eax + 7], dh      ; base high byte

    pop  ebp
    ret
