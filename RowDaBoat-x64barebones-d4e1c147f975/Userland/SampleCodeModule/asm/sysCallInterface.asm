GLOBAL _sys_write
GLOBAL _sys_read
GLOBAL _sys_drawSprite
GLOBAL _sys_getMillis
GLOBAL _sys_getScreenWidth
GLOBAL _sys_getScreenHeight
GLOBAL _sys_cleanScreen
GLOBAL _sys_beep
GLOBAL _sys_getTime
GLOBAL _sys_setFontSize
GLOBAL _sys_getFontSize
GLOBAL _sys_formatWrite
GLOBAL _sys_getScreenBpp
GLOBAL _sys_getSavedRegisters
GLOBAL _sys_malloc
GLOBAL _sys_free

section .text
;void _sys_write(int fd, char *str, int lenght);
_sys_write:
    push rbp
    mov rbp, rsp

    mov rax, 0      ;id de write
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;Reads up to count chars into buf, returns amount of chars read
;int sys_read(int fd, uint16_t *buf, int count);
_sys_read:
    push rbp
    mov rbp, rsp

    mov rax, 1      ;id de read
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;void _sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width*3]);
_sys_drawSprite:
    push rbp
    mov rbp, rsp

    mov rax, 2      ;id 
    int 80h
    
    mov rsp, rbp
    pop rbp
    ret

;uint32_t _sys_getMillis()
_sys_getMillis:
    push rbp
    mov rbp, rsp

    mov rax, 3      ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret
;void sys_cleanScreen();
_sys_cleanScreen:
    push rbp
    mov rbp, rsp

    mov rax, 4
    int 80h

    mov rsp, rbp
    pop rbp
    ret

_sys_getScreenWidth:
    push rbp
    mov rbp, rsp

    mov rax, 5      ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret

_sys_getScreenHeight:
    push rbp
    mov rbp, rsp

    mov rax, 6      ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;void _sys_beep(uint16_t frequency)
_sys_beep:
    push rbp
    mov rbp, rsp

    mov rax, 7
    int 80h

    mov rsp, rbp
    pop rbp
    ret
    
;void _sys_getTime(long* hour, long* min, long* seg);
_sys_getTime:
    push rbp
    mov rbp, rsp

    mov rax, 8     ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret 
    
;void sys_setFontSize(uint8_t size);
_sys_setFontSize:
    push rbp
    mov rbp, rsp

    mov rax, 9
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;uint8_t sys_getFontSize();
_sys_getFontSize:
    push rbp
    mov rbp, rsp

    mov rax, 10
    int 80h

    mov rsp, rbp
    pop rbp
    ret


;void sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color,uint16_t row, uint16_t col){
_sys_formatWrite:
    push rbp
    mov rbp, rsp

    mov rax, 11     ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;uint8_t sys_getScreenBpp();
_sys_getScreenBpp:
    push rbp
    mov rbp, rsp
    mov rax, 12     ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;  void sys_getSavedRegisters(uint64_t registers[17]);
_sys_getSavedRegisters:
    push rbp
    mov rbp, rsp
    
    mov rax, 13     ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;void *      sys_malloc(uint64_t size);
_sys_malloc:
    push rbp
    mov rbp, rsp
    
    mov rax, 14     ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;void        sys_free(void * ptr);
_sys_free:
    push rbp
    mov rbp, rsp
    
    mov rax, 15     ;id 
    int 80h

    mov rsp, rbp
    pop rbp
    ret