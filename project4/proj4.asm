;;; layout of the structure
%define TITLE_OFFSET 0
%define AUTHOR_OFFSET 48
%define SUBJECT_OFFSET 96
%define YEAR_OFFSET 116
%define NEXT_OFFSET 120

;;; our usual system call stuff
%define WRITE 4
%define STDOUT 1

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
extern library

;;; eax should point to the string. on return, eax is the length
stringLength:
        push esi
        mov esi, eax
        mov eax, 0

.loop:
        cmp [esi], byte 0
        je .end

        inc eax
        inc esi
        jmp .loop

.end:
        pop esi
        ret

_start:                         ; the program actually starts here

        ;; print the first author in the library
        mov eax, library
        add eax, AUTHOR_OFFSET
        call stringLength       ; after this, EAX will have the length of the author name

        mov edx, eax            ; copy it to the count register for the system call
        mov eax, WRITE
        mov ebx, STDOUT
        lea ecx, [eax + AUTHOR_OFFSET]  ; Load-Effective-Address computes the address in
                                        ; the brackets and returns it instead of looking it up.
        int 80h


        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
