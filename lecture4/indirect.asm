[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance
HelloWorld:     db "Hello, world!", 10

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where are program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here
	mov eax, [HelloWorld]
	cmp eax, "Hell"
	je TrueLabel
	jmp FalseLabel

TrueLabel:
        mov eax, 1              ; return the value as an error code
        mov ebx, 1
        int 80h

FalseLabel:
        mov eax, 1              ; return the value as an error code
        mov ebx, 0
        int 80h

