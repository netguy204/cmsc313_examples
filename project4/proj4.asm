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

newline:        db 10

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it
extern library
global authorsForDate

printNewline:
        push eax
        push ebx
        push ecx
        push edx

        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, newline
        mov edx, 1
        int 80h

        pop edx
        pop ecx
        pop ebx
        pop eax
        ret

;;; eax should point to the string. on return, eax is the length
stringLength:
;;; Students: Feel free to use this code in your submission but you
;;; must add comments explaining the code to prove that you
;;; know how it works.

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

;;; this label will be called as a subroutine by the code in driver.asm
authorsForDate:
        ;; protect the registers we use
        push eax
        push ebx
        push ecx
        push edx
        push esi

        ;; print the first author in the library
        mov esi, [library]
        lea eax, [esi + AUTHOR_OFFSET]  ; Load-Effective-Address computes the address in
                                        ; the brackets and returns it instead of looking it up.

        call stringLength       ; after this, EAX will have the length of the author name

        mov edx, eax            ; copy it to the count register for the system call
        mov eax, WRITE
        mov ebx, STDOUT
        lea ecx, [esi + AUTHOR_OFFSET]
        int 80h

        call printNewline

        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        ret
