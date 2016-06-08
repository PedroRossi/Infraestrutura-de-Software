; hello.asm       print a string using printf
; Assemble:	  nasm -f elf64 -l Q4.lst  Q4.asm
; Link:		  gcc -o Q4  Q4.o

; como imprimir algo
;mov	rdi,fmt
;mov	rsi,spc
;mov	rax,0
;call printf

extern	printf		; the C function, to be called

section .data     ; Data section, initialized variables

  ast: db "*", 0	; C string needs 0
  spc: db " ", 0
  fmt: db "%s", 0  ; The printf format,'0'
  fmtln: db "%s", 10, 0 ; The printf format, "\n",'0'
  H: resb 4
  W: resb 4

section .text      ; Code section.

global main		    ; the standard gcc entry point
main:			 	      ; the program label for the entry point
	push    rbp		  ; set up stack frame, must be aligned

	;main_loop:

  mov	rdi,fmt
	mov	rsi,ast
	mov	rax,0		    ; or can be  xor  rax,rax
	call printf		  ; Call C function

	mov	rdi,fmtln
	mov	rsi,ast
	mov	rax,0
	call printf		  ; Call C function

	;jne main_loop

	pop  rbp		    ; restore stack

	mov	rax,0		    ; normal, no error, return value
	ret			; return
