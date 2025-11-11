[bits 16]
global print_string
; Print string in real mode using BIOS interrupts
; Input: bx = address of null-terminated string

print_string:
    pusha
    mov ah, 0x0e           ; BIOS teletype output
.loop:
    mov al, [bx]           ; Load character
    cmp al, 0              ; Check for null terminator
    je .done
    int 0x10               ; Print character
    add bx, 1              ; Next character
    jmp .loop
.done:
    mov al, 0x0a           ; Newline
    int 0x10
    mov al, 0x0d           ; Carriage return
    int 0x10
    popa
    ret
