[org 0x7c00]
; SUB OS Bootloader - Stage 1
; Simple bootloader that prints "SUB OS" and prepares to load kernel

KERNEL_OFFSET equ 0x1000    ; Memory offset where kernel will be loaded

    mov [BOOT_DRIVE], dl    ; BIOS stores boot drive in dl
    
    ; Set up stack
    mov bp, 0x9000
    mov sp, bp
    
    ; Print boot message
    mov bx, MSG_REAL_MODE
    call print_string
    
    ; Detect memory
    call detect_memory
    
    ; Load kernel from disk
    call load_kernel
    
    ; Switch to protected mode
    call switch_to_pm
    
    jmp $                   ; Should never reach here

%include "boot/print_string.asm"
%include "boot/disk_load.asm"
%include "boot/memory_detect.asm"
%include "boot/gdt.asm"
%include "boot/print_string_pm.asm"
%include "boot/switch_to_pm.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string
    
    mov bx, KERNEL_OFFSET   ; Read from disk and store in 0x1000
    mov dh, 20              ; Read 20 sectors (increased for larger kernel)
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

; Removed conflicting BEGIN_PM definition from boot.asm (now only provided in boot/switch_to_pm.asm)

; Data
BOOT_DRIVE      db 0
MSG_REAL_MODE   db "SUB OS Bootloader Started (16-bit Real Mode)", 0
MSG_LOAD_KERNEL db "Loading SUB OS Kernel...", 0
MSG_PROT_MODE   db "Successfully entered 32-bit Protected Mode", 0

; Boot sector padding
times 510-($-$$) db 0
dw 0xaa55
