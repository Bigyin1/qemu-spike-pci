.section .text
.global _start

_start:
    la sp, _stack_top
    
    call prog

    la a1, exit_cmd

    la a2, tohost
    sd a1, 0(a2)


.section .data    
exit_cmd:
    .dword 93
    .fill 7, 8, 0
    .dword fromhost
