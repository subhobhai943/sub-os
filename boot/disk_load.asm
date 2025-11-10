[bits 16]
; Load sectors from disk
; Input: dh = number of sectors, dl = drive number, bx = destination

disk_load:
    push dx
    
    mov ah, 0x02           ; BIOS read sector function
    mov al, dh             ; Number of sectors to read
    mov ch, 0x00           ; Cylinder 0
    mov dh, 0x00           ; Head 0
    mov cl, 0x02           ; Start from sector 2 (after boot sector)
    
    int 0x13               ; BIOS disk interrupt
    jc disk_error          ; Jump if error (carry flag set)
    
    pop dx
    cmp dh, al             ; Check if sectors read = sectors requested
    jne disk_error
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $

DISK_ERROR_MSG db "Disk read error!", 0
