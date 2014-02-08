[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here

        mov eax, 4              ; familiar pair of operations
        add eax, 4

        mov ebx, eax            ; return the result as an error code
        mov eax, 1
        int 80h
