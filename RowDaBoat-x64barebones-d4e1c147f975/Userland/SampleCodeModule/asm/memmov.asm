GLOBAL memmov

section .text

;Move bytes from one memory region to another
;Regions can overlap but only if destination < src
;checks if all arguments are aligned to 64 bits and does a fast copy if they are.
;could also implement a 32 and 16 bit aligment check, not needed for now

;Arguments:
;   rdi : destination address
;   rsi : source address
;   rdx : Quantity of bytes to copy
;Returns:
;   rax : destination address
memmov:
    push rbp
	mov rbp, rsp
    push rcx
    push r14
    mov rcx, rdx    ;counter for rep

    cmp rdi, rsi    ;already the same position
    je .end

    mov rax, rdi
    push rax        ;stores the destination address

    mov r14, 64     ;64 bit check
    mov rdx,0
    div r14          ;checks destination (rdi) alignment
    cmp rdx, 0      ;remainder of the division
    jne .slow

    mov rax, rsi
    mov rdx, 0
    div r14          ;checks source (rsi) alignment
    cmp rdx, 0      ;remainder of the division
    jne .slow

    mov rax, rcx
    mov rdx,0
    div r14          ;checks destination (rdi) alignment
    cmp rdx, 0      ;remainder of the division
    jne .slow

    ;arguments are aligned, fast mov. Divide lenght by 8 (8 bytes in a qword)
    mov r14, 8
    mov rax, rcx
    mov rdx, 0
    div r14
    mov rcx, rax
  
    rep movsq
    jmp .end

.slow:
    rep movsb

.end:
    pop rax         ;returns the destination address
    pop r14
    pop rcx
    mov rsp, rbp
    pop rbp
    ret
