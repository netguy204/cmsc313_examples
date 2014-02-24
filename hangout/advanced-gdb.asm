%define WRITE 4
%define STDOUT 1

[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

data:   db "abcd"

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

buf:    resb 4

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here

        ;; look at eax using p/x
        ;; look at &data using x/4c
        ;; look at &data again using x/x
        mov eax, [data]

        ;; grab a byte at a time and shuffle them around. we
        ;; accomplish the same thing as bitmanip but in a more
        ;; obvious way. Apparnt downside is that there are a lot of reads
        ;; from memory but cache will make this have virtually no impact.
        mov bl, [data]
        mov [buf+3], bl
        mov bl, [data+1]
        mov [buf+2], bl
        mov bl, [data+2]
        mov [buf+1], bl
        mov bl, [data+3]
        mov [buf], bl

        ;; look at eax using p/x
        ;; look at &data using x/4c
        ;; look at &data again using x/x
        mov eax, [buf]


        ;; print stuff out, just for fun
        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, buf
        mov edx, 4
        int 80H

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
