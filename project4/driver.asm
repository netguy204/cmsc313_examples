%define WRITE 4
%define STDOUT 1

[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

gapString:      db "=========", 10
gapStringLen:   EQU $-gapString

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it
extern authorsForDate

printGap:
        push eax
        push ebx
        push ecx
        push edx

        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, gapString
        mov edx, gapStringLen
        int 80h

        pop edx
        pop ecx
        pop ebx
        pop eax
        ret

_start:                         ; the program actually starts here
        mov eax, 2011
        call authorsForDate
        call printGap

        mov eax, 2012
        mov ebx, 2001
        call authorsForDate
        call printGap

        ;; making sure the registers were actually protected
        mov eax, ebx
        call authorsForDate

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
