[bits 32]
; System Call Handler - INT 0x80
; SUB OS - User/Kernel interface

global syscall_entry
extern syscall_handler

syscall_entry:
    ; Save all registers
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    
    ; Call C handler
    ; EAX = syscall number
    ; EBX = arg1, ECX = arg2, EDX = arg3
    push edx    ; arg3
    push ecx    ; arg2
    push ebx    ; arg1
    push eax    ; syscall number
    call syscall_handler
    add esp, 16 ; Clean up stack
    
    ; Restore registers
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    
    ; Return value in EAX
    iret
