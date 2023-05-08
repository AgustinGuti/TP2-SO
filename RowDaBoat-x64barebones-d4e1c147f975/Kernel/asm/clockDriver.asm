section .text

global get_seconds
get_seconds:
   push rbp
   mov rbp, rsp

   mov al, [secondsID]
   out 70h, al
   mov rax, 0
   in al, 71h

   mov rsp, rbp
   pop rbp
   ret

global get_minute
get_minute:
   push rbp
   mov rbp, rsp

   mov rax, 0
   mov al, [minutesID]
   out 70h, al
   in al, 71h

   mov rsp, rbp
   pop rbp
   ret

global get_hour
get_hour:
   push rbp
   mov rbp, rsp

   mov rax, 0
   mov al, [hoursID]
   out 70h, al
   in al, 71h

   mov rsp, rbp
   pop rbp
   ret   


section .data
    secondsID db 00
    minutesID db 02
    hoursID db 04
    dayOfWeekID db 06
    dayOfMonthID db 07
    monthID db 08
    yearID db 09
