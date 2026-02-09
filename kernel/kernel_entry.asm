[bits 32]
[global _start]
[extern kernel_main]       ; Main C kernel function

_start:
    call kernel_main           ; Call kernel main function
    jmp $                      ; Hang if kernel returns
