[bits 32]
[extern main]              ; Main C function

call main                  ; Call kernel main function
jmp $                      ; Hang if kernel returns
