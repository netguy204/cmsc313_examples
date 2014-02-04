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
        mov eax, 4
        mov ebx, 1
        mov ecx, HelloMsg
        mov edx, HelloLen
        int 80H

        mov eax, 1
        mov ebx, 0
        int 80H
