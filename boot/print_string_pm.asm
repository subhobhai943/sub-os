[bits 32]
global print_string_pm
; Print string in protected mode (VGA text mode)
; Input: ebx = address of null-terminated string

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY
.loop:
    mov al, [ebx]              ; Load character
    mov ah, WHITE_ON_BLACK     ; Set attribute
    cmp al, 0                  ; Check for null terminator
    je .done
    mov [edx], ax              ; Write to video memory
    add ebx, 1
    add edx, 2
    jmp .loop
.done:
    popa
    ret
