[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance
Msg: db "The number is: %d",10,0

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global main                    ; make start global so ld can find it
extern printf

main:                          ; this gets called from code in crt0
;; argc and argv are available on our stack if we want them

        ;; printf("The number is: %d", 15);
        push 15
        push Msg
        call printf
        add esp, 8

        mov eax, 0              ; return code goes in eax
        ret
