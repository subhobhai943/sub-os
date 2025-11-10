[bits 16]
; Memory detection using BIOS INT 0x15, EAX=0xE820
; Store memory map at 0x5000

detect_memory:
    pusha
    mov di, 0x5000          ; Destination for memory map
    xor ebx, ebx            ; EBX must be 0 to start
    xor bp, bp              ; BP will count entries
    mov edx, 0x534D4150     ; 'SMAP' signature
    mov eax, 0xE820
    mov ecx, 24             ; Size of buffer
    int 0x15
    jc .failed
    mov edx, 0x534D4150     ; Some BIOSes trash this
    cmp eax, edx
    jne .failed
    test ebx, ebx           ; If EBX=0, only 1 entry (not useful)
    je .failed
    jmp .check_entry

.loop:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    jc .done                ; Carry set = end of list
    mov edx, 0x534D4150     ; Repair SMAP

.check_entry:
    jcxz .skip_entry        ; Skip 0 length entries
    cmp cl, 20              ; Minimum 20 bytes
    jbe .no_text
    test byte [di + 20], 1  ; If bit 0 clear, ignore
    je .skip_entry

.no_text:
    mov ecx, [di + 8]       ; Get lower 32 bits of length
    or ecx, [di + 12]       ; OR with upper 32 bits
    jz .skip_entry          ; Skip 0 length entries
    inc bp                  ; Increment entry count
    add di, 24              ; Move to next entry

.skip_entry:
    test ebx, ebx           ; If EBX=0, list complete
    jne .loop

.done:
    mov [0x5000 + 24*100], bp  ; Store entry count
    popa
    ret

.failed:
    mov bx, MEMORY_DETECT_FAILED
    call print_string
    popa
    ret

MEMORY_DETECT_FAILED db "Memory detection failed, using defaults", 0
