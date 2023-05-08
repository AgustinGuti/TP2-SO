GLOBAL _speaker_tone
GLOBAL _speaker_off

section .text
 ;rdi - Note frequency, only 16 bit effective
_speaker_tone:
    push rbp
    mov rbp, rsp

    mov rax, rdi
    mov cx, ax

    mov al, 182             ;10 11 011 0        16 bit binary - mode 3 - lobyte/hibyte  -  channel 2  https://wiki.osdev.org/Programmable_Interval_Timer
    out 43h, al
    mov ax, cx                      ; Set up frequency
    out 42h, al
    mov al, ah
    out 42h, al

    in al, 61h                      ; Switch PC speaker on
    or al, 03h
    out 61h, al

    mov rsp, rbp
    pop rbp
    ret


_speaker_off:
    push rbp
    mov rbp, rsp

    in al, 61h
    and al, 0FCh
    out 61h, al

    
    mov rsp, rbp
    pop rbp
    ret
