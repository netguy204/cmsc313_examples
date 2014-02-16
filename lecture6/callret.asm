[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

hello:  db "Hello, World!", 10
hello_len:      EQU $-hello

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

sayhi:
        ;; protect the old values in eax-edx
        push eax
        push ebx
        push ecx
        push edx

        mov eax, 4
        mov ebx, 1
        mov ecx, hello
        mov edx, hello_len
        int 80h

        pop edx
        pop ecx
        pop ebx
        pop eax

        ret

_start:                         ; the program actually starts here
        ;; say hi a few times
        mov eax, 10
.loop:
        cmp eax, 0
        je .loopend

        call sayhi
        dec eax
        jmp .loop

.loopend:

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
