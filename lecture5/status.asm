[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

binbuf:         resb 1
binnum:         resb 4
bincount:       resb 4

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

printbinary:
        ;; display the byte stored in al as binary
        mov [binnum], byte al
        mov [bincount], byte 7

.loop:
        mov al, [binnum]
        shr al, 7
        add al, '0'
        mov [binbuf], al

        ;; print the value
        mov eax, 4
        mov ebx, 1
        mov ecx, binbuf
        mov edx, 1
        int 80H

        ;; shift off the bit we just printed
        shl [binnum], 1
        cmp [bincount], byte 0
        je .end

        dec byte [bincount]
        jmp .loop

.end:
        ;; print a newline
        mov [binbuf], byte 10
        mov eax, 4
        mov ebx, 1
        mov ecx, binbuf
        mov edx, 1
        int 80H

        ;; return to caller
        ret

_start:                         ; the program actually starts here
        ;; add two numbers using a byte-sized register such that we get an
        ;; overflow
        mov ah, 200
        add ah, 200

        ;; load the status bits (SF:ZF:0:AF:0:PF:1:CF)
        ;; SF - Sign Flag
        ;; ZF - Zero Flag
        ;; AF - Auxiliary Carry Flag
        ;; PF - Parity Flag
        ;; XF - Carry Flag
        lahf
        mov al, ah

        ;; print out the value in al in binary to STDOUT
        call printbinary

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
