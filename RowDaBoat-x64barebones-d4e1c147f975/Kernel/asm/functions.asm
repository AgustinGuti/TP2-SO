
GLOBAL triggerTimer
GLOBAL getRIP
GLOBAL getRSP

triggerTimer:
    int 20h
    ret

getRIP:
    mov rax, [rbp+8]
    ret

getRSP:
    mov rax, [rbp]
    ret