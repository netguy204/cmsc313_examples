[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

printnum_buf:   resb 32
printnum_buf_end EQU $-1
printnum_buf_len EQU $-printnum_buf

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

fibonacci:
        ;; if ( eax == 0 || eax == 1) {
        ;;   return 1;
        ;; } else {
        ;;   return fib(eax - 1) + fib(eax - 2);
        ;; }
        cmp eax, 1
        jle .return1

        push eax                ; stash eax

        sub eax, 1
        call fibonacci          ; fib(eax - 1)

        pop ebx                 ; restore the value of eax we stashed
        push eax                ; retain the result

        mov eax, ebx
        sub eax, 2
        call fibonacci          ; fib(eax - 2)

        pop ebx                 ; restore fib(eax-1)
        add eax, ebx            ; fib(eax-1) + fib(eax-2)

        ret

.return1:
        mov eax, 1
        ret

_start:                         ; the program actually starts here

        mov eax, 10
        call fibonacci

        push eax
        call printnum

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt







;;; Here's some fun code you may find interesting. A looping
;;; implementation of the division operation and converting a
;;; multidigit number to its ascii representation.


;;; divide the two numbers on the stack. The quotient will be in eax
;;; and the remainder will be in ecx.
divpositive:
        push ebx

        mov eax, 0              ; result
        mov ebx, [esp + 8]      ; divisor
        mov ecx, [esp + 12]     ; dividend, and remainder

.loop:
        cmp ebx, ecx
        jg .end

        sub ecx, ebx
        inc eax
        jmp .loop

.end:
        pop ebx
        ret

;;; returns a pointer to a buffer with the string representation of
;;; the number given. Pointer in eax, length in ecx
numstring:
        ;; while the number is greater than zero, divide it by 10, put
        ;; the remainder at the end of our buffer, and continue
        push ebx
        push edi

        mov eax, [esp + 12]      ; number to print
        mov edi, printnum_buf_end ; tail end of our string
        mov ebx, 0              ; offset from end

.loop:
        cmp eax, 0
        je .end

        push eax
        push 10
        call divpositive
        add esp, 8              ; clean the stack

        add cl, '0'             ; shift remainder into ascii
        mov [edi], cl           ; copy to buffer
        dec edi                 ; move to next position in buffer

        ;; eax contains the quotient so we're ready to begin again
        jmp .loop

.end:
        ;; return the results
        mov eax, edi
        mov ecx, printnum_buf_end
        sub ecx, eax

        add eax, 1

        ;; restore protected registers
        pop edi
        pop ebx

        ret

printnum:
        push eax
        push ebx
        push ecx
        push edx

        mov eax, [esp + 20]
        push eax
        call numstring
        add esp, 4

        mov [eax + ecx], byte 10 ; append a newline
        inc ecx                  ; and make sure we print it

        mov edx, ecx            ; get size in the right place
        mov ecx, eax            ; get pointer in the right place
        mov eax, 4              ; sys_write
        mov ebx, 1              ; stdout
        int 80h

        pop edx
        pop ecx
        pop ebx
        pop eax

        ret
