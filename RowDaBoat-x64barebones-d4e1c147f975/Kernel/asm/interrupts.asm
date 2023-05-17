GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL _setupSysCalls
GLOBAL _sysCallHandler

GLOBAL saveRegisters
GLOBAL saveCurrentRegs

GLOBAL savedRegisters
GLOBAL haveSaved

EXTERN irqDispatcher
EXTERN exceptionDispatcher

EXTERN printf

EXTERN loader

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1

	mov [registers+8*0], 	rax
	mov [registers+8*1], 	rbx
	mov [registers+8*2], 	rcx
	mov [registers+8*3], 	rdx
	mov [registers+8*4], 	rsi
	mov [registers+8*5], 	rdi
	mov [registers+8*6], 	rbp
	mov [registers+8*7], 	r8
	mov [registers+8*8], 	r9
	mov [registers+8*9], 	r10
	mov [registers+8*10], 	r11
	mov [registers+8*11], 	r12
	mov [registers+8*12], 	r13
	mov [registers+8*13], 	r14
	mov [registers+8*14], 	r15

	mov rax, rsp  
	mov [registers+8*15], rax
	mov rax, [rsp]
	mov rdx, rax			;store RIP
	mov [registers+8*16], rax

	;Check if RIP (rdx) is greater than 0x400000, then continue. If it is not, the exception came from kernel space and we can't recover, enter a halt loop
	cmp rdx, 0x400000	; start of userland
	jge .retKernel

.loop:
	call haltcpu
	jmp .loop	

.retKernel:
	mov rdi, %1 ; exception number
	mov rsi, registers 
	call exceptionDispatcher

	;Try to restore original registers
	mov rax, [startRegisters+8*0]
	mov rbx, [startRegisters+8*1]
	mov rcx, [startRegisters+8*2]
	mov rdx, [startRegisters+8*3]
	mov rsi, [startRegisters+8*4]
	mov rdi, [startRegisters+8*5]
	mov rbp, [startRegisters+8*6]
	mov r8,  [startRegisters+8*7]
	mov r9,  [startRegisters+8*8]
	mov r10, [startRegisters+8*9]
	mov r11, [startRegisters+8*10]
	mov r12, [startRegisters+8*11]
	mov r13, [startRegisters+8*12]
	mov r14, [startRegisters+8*13]
	mov r15, [startRegisters+8*14]

	mov rax, [startRegisters+8*16]	;RIP, returns to kernel before calling userland.  We can't recover from the exception, so we restart the execution.
	mov [rsp], rax

	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
	mov rsp, rbp
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
	mov rsp, rbp
    pop     rbp
    retn

_sysCallHandler: 
	push rbp
	mov rbp, rsp
	push rbx

	cmp rax, [maxSysCall]		;Si es mayor a la ultima syscall que tengo, aborto
	jg .end

	push rdx
	mov rdx, 0
	mov rbx, 8
	mul rbx						;Donde esta el puntero al que quiero ir
	pop rdx
	call [sysCallsPointers+rax]		;Ejecuto la funcion, con los mismos parametros que me habian pasado
.end:
	pop rbx
	mov rsp, rbp
	pop rbp
	iretq


_setupSysCalls:
	push rbp
	mov rbp, rsp
	push rcx
	push rbx

	mov rcx, rdi			;Cuantos punteros
	dec rdi					;Empieza en 0 el arreglo
	mov [maxSysCall], rdi	;Guardo cuantos syscalls tengo
	mov rbx, rsi			;Arreglo de punteros
	mov rax, 0				;Contador

.loop:	
	cmp rcx, 0				;Si ya termino
	je .end
	mov r12, [rsi+rax]		;El puntero que voy a guardar
	mov [sysCallsPointers+rax], r12		;Lo guardo
	add rax, 8				;Cada puntero ocupa 8
	dec rcx					;uno menos
	jmp .loop

.end:
	pop rcx
	pop rbx
	mov rsp, rbp
	pop rbp
	ret

;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	push rax

	in al, 60h	;Reads data from keyboard
	cmp rax, 0x1D ;LCTRL make scancode

	jne .continue 
	mov [haveSaved], byte 1
	mov rax, [rsp]				;original rax was stored in rsp
	mov [savedRegisters+8*0], 	rax	
	mov [savedRegisters+8*1], 	rbx
	mov [savedRegisters+8*2], 	rcx
	mov [savedRegisters+8*3], 	rdx
	mov [savedRegisters+8*4], 	rsi
	mov [savedRegisters+8*5], 	rdi
	mov [savedRegisters+8*6], 	rbp
	mov [savedRegisters+8*7], 	r8
	mov [savedRegisters+8*8], 	r9
	mov [savedRegisters+8*9], 	r10
	mov [savedRegisters+8*10], 	r11
	mov [savedRegisters+8*11], 	r12
	mov [savedRegisters+8*12], 	r13
	mov [savedRegisters+8*13], 	r14
	mov [savedRegisters+8*14], 	r15

	mov rax, rsp  
	add rax, 8
	mov [savedRegisters+8*15], rax	;RSP
	mov rax, [rsp+8]   ; RSP contains the return adress, so we get the RIP
	mov [savedRegisters+8*16], rax

.continue:
	mov rsi, rax			;pass the read data as parameter
	pop rax
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

saveRegisters:
	mov [startRegisters+8*0], 	rax
	mov [startRegisters+8*1], 	rbx
	mov [startRegisters+8*2], 	rcx
	mov [startRegisters+8*3], 	rdx
	mov [startRegisters+8*4], 	rsi
	mov [startRegisters+8*5], 	rdi
	mov [startRegisters+8*6], 	rbp
	mov [startRegisters+8*7], 	r8
	mov [startRegisters+8*8], 	r9
	mov [startRegisters+8*9], 	r10
	mov [startRegisters+8*10], 	r11
	mov [startRegisters+8*11], 	r12
	mov [startRegisters+8*12], 	r13
	mov [startRegisters+8*13], 	r14
	mov [startRegisters+8*14], 	r15

	mov [startRegisters+8*15], rsp	;RSP
	mov rax, [rsp]   ; RSP contains the return adress, so we get the RIP
	mov [startRegisters+8*16], rax

	push r12
	push r13

	mov [isStackSaved], byte 1
	mov r12, 3		; saves the first 3 entries of the stack
	mov r13, 24		; Initial offset, skips 2 pushes and local ret address

.loop:
	mov rdx, [rsp + r13]
	mov [startStack + r13 - 24], rdx
	add r13, 8
	dec r12
	cmp r12, 0
	jne .loop
	
	pop r13
	pop r12

	ret

GLOBAL restoreStack
restoreStack:		;restores the first 3 entries of the stack

	;Change rsp pointer without losing its return value
	mov rax, [rsp]					;Save return address
	mov rsp, [startRegisters+8*15]	;Move stack pointer to original value
	mov [rsp], rax					;Restore return address

	push r12
	push r13
	
	mov r12, [isStackSaved]	
	cmp r12, 0		;Checks if the stack was saved
	je .end

	mov r12, 3
	mov r13, 24		;Do not touch local return addres, and 2 local pushes

.loop:
	
	mov rdx, [startStack + r13 - 24]
	mov [rsp + r13], rdx
	add r13, 8
	dec r12

	cmp r12, 0
	jne .loop

.end:	

	pop r13
	pop r12

	ret


section .data
	haveSaved dq 0
	isStackSaved dq 0

section .bss
	maxSysCall resb 15
	sysCallsPointers resb 128		;Alcanza para 16 sysCalls
	registers resq 17

	startRegisters resq 17
	startStack resq 3

	savedRegisters resq 17

