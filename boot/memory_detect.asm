[bits 16]
; Memory detection using BIOS INT 0x15, EAX=0xE820
; Stores memory map at physical address 0x5000

detect_memory:
    pusha
    mov di, 0x5000          ; Destination for memory map entries
    xor ebx, ebx            ; EBX must be 0 to start
    xor bp,  bp             ; BP counts valid entries
    mov edx, 0x534D4150     ; 'SMAP' signature
    mov eax, 0xE820
    mov ecx, 24             ; Each entry is 24 bytes
    int 0x15
    jc  .failed
    mov edx, 0x534D4150     ; Restore - some BIOSes trash EDX
    cmp eax, edx
    jne .failed
    test ebx, ebx           ; EBX=0 means only 1 entry (useless)
    je  .failed
    jmp .check_entry

.loop:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    jc  .done               ; Carry set = end of list
    mov edx, 0x534D4150     ; Repair 'SMAP'

.check_entry:
    jcxz .skip_entry        ; Skip 0-length entries
    cmp  cl, 20
    jbe  .no_ext
    test byte [di + 20], 1  ; ACPI extended attrib: bit 0 clear = ignore
    je   .skip_entry

.no_ext:
    mov ecx, [di + 8]       ; Lower 32 bits of length
    or  ecx, [di + 12]      ; OR upper 32 bits
    jz  .skip_entry         ; Skip zero-length
    inc bp
    add di, 24

.skip_entry:
    test ebx, ebx           ; EBX=0 means list complete
    jne  .loop

.done:
    mov [0x5000 + 24*100], bp   ; Store entry count after map
    popa
    ret

.failed:
    mov bx, MEMORY_DETECT_FAILED
    call print_string
    popa
    ret

MEMORY_DETECT_FAILED db "Memory detection failed, using defaults", 0
