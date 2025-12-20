[bits 32]
[extern kernel_main]       ; Main C kernel function

call kernel_main           ; Call kernel main function
jmp $                      ; Hang if kernel returns
