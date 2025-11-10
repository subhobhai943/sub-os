[bits 32]
; User Mode Entry
; SUB OS - Switch to Ring 3

global enter_usermode

enter_usermode:
    cli
    
    mov eax, [esp+4]
    mov ebx, [esp+8]
    
    mov cx, 0x23
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    
    push 0x23
    push ebx
    
    pushf
    pop ecx
    or ecx, 0x200
    push ecx
    
    push 0x1B
    push eax
    
    iret
