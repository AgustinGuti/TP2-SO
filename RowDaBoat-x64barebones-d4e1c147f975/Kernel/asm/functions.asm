
GLOBAL triggerTimer
GLOBAL getRIP
GLOBAL getRSP

triggerTimer:
    ;cli
    int 20h
   ; sti
    ret

getRIP:
    mov rax, [rbp+8]
    ret

getRSP:
    mov rax, [rbp]
    ret