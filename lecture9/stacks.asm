[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

valueMsg:       db "ESP = %X", 10, 0
memoryMsg:      db "[ESP] = %X", 10, 0

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global main                   ; make start global so ld can find it
extern printf

main:                         ; the program actually starts here
        push esp
        push valueMsg
        call printf
        add esp, 8

        push dword [esp]
        push memoryMsg
        call printf
        add esp, 8

        mov eax, 0xafafafaf

        push ax

        push esp
        push valueMsg
        call printf
        add esp, 8

        push dword [esp]
        push memoryMsg
        call printf
        add esp, 8



        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
