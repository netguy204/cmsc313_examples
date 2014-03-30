[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global add2

add2:
        ;; top of the stack is the return address. next is our first argument
        mov eax, [esp + 4]
        add eax, [esp + 8]

        ;; return value goes into eax so we're done
        ret
