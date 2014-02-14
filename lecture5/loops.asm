[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance
input:          dd 1, 2, 3, 4
inputcnt        equ $-input

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

output: resb inputcnt

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here

        ;; add 1 to everything in the input
        ;; for(int i = 0; i < inputcnt; ++i) {
        ;;   output[i] = input[i] + 1;
        ;; }

        ;; initialize
        mov esi, input
        mov edi, output
        mov eax, 0              ; i = 0

.looptest:
        cmp eax, inputcnt       ; i < inputcnt
        jl .loopbody            ; yes, loop body
        jmp .loopexit           ; otherwise, loop finished

.loopbody:
        mov ebx, [esi + 4*eax]    ; ebx = input[i]
        add ebx, 1              ; ebx += 1
        mov [edi + 4*eax], ebx    ; output[i] = ebx

        inc eax                 ; ++i
        jmp .looptest

.loopexit:

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
