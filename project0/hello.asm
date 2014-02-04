[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

HelloMsg:       db "Hello, world!", 10
HelloLen:       equ $-HelloMsg

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where are program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here

        mov eax, 4              ; sys_write system call
        mov ebx, 1              ; stdout is the target
        mov ecx, HelloMsg       ; adddress of the start of the message
        mov edx, HelloLen       ; length of the message
        int 80H                 ; kernel interrupt

        mov eax, 1              ; sys_exit
        mov ebx, 0              ; status code to return
        int 80H                 ; kernel interrupt
