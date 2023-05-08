GLOBAL has_key
has_key:
    push rbp
    mov rbp, rsp
    push rbp

    mov rax, 0;
    in al, 64h
    mov bl, 1       ;mascara, deja solo el ultimo bit
    AND al, bl      ;mascara, deja solo el ultimo bit
    
    pop rbp
    mov rsp, rbp
    pop rbp
    ret


;Obtiene la tecla que fue presionada, o -1 si no habia datos esperando
 GLOBAL get_key
 get_key:
     push rbp
     mov rbp, rsp
     mov rax, 0
 
     call has_key
     cmp al, 1           ;verifico si habia tecla
     je .key_ready
     mov rax, -1         ; Si no, retorno -1
     jmp .end
 
 .key_ready:
     in al, 60h         ;Leo el dato del teclado
 
 .end:
     mov rsp, rbp
     pop rbp
     ret