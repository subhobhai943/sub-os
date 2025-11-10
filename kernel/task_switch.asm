[bits 32]
; Task Switching
; SUB OS - Low-level context switching

global switch_to_task
global read_eip

; Read current instruction pointer
read_eip:
    pop eax
    jmp eax

; Switch to another task
; Parameters: EAX = pointer to current process, EBX = pointer to next process
switch_to_task:
    cli
    
    mov ecx, [esp+4]   ; ECX = current process
    mov edx, [esp+8]   ; EDX = next process
    
    ; Check if current process exists
    test ecx, ecx
    jz .load_context
    
    ; Save current process context
    mov [ecx+4], ebx   ; Save EBX
    mov [ecx+8], ecx   ; Save ECX (we modify it)
    mov [ecx+12], edx  ; Save EDX (we modify it)
    
    mov ebx, [esp]     ; Get return address
    mov [ecx+32], ebx  ; Save EIP
    
    mov [ecx+28], esp  ; Save ESP
    mov [ecx+30], ebp  ; Save EBP
    
    pushfd
    pop ebx
    mov [ecx+36], ebx  ; Save EFLAGS
    
.load_context:
    ; Load next process context
    mov eax, [edx+0]   ; Restore EAX
    mov ebx, [edx+4]   ; Restore EBX
    mov ecx, [edx+8]   ; Restore ECX
    
    mov esp, [edx+28]  ; Restore ESP
    mov ebp, [edx+30]  ; Restore EBP
    
    mov esi, [edx+36]  ; Get EFLAGS
    push esi
    popfd              ; Restore EFLAGS
    
    ; Load page directory
    mov esi, [edx+40]  ; Get CR3
    mov cr3, esi       ; Switch page directory
    
    ; Jump to saved EIP
    mov esi, [edx+32]
    jmp esi
