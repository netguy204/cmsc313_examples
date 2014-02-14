[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

my_array:       db 4, 9, 2, 6
my_array_size:  equ $ - my_array

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

index:  resd    1
buffer: resb    1

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here
        mov [index], dword 0

.loop:
        cmp [index], dword my_array_size
        jge .loopend

        mov eax, [index]
        mov al, [my_array + eax]
        add al, '0'
        mov [buffer], al

        mov eax, 4
        mov ebx, 1
        mov ecx, buffer
        mov edx, 1
        int 80h

        inc dword [index]
        jmp .loop

.loopend:
        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
