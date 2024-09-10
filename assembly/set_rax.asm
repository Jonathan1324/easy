; set_rax.asm
global setRAX

section .text
setRAX:
    ; Arguments: 
    ;   rdi - the value to set in RAX
    mov rax, rdi
    ret
