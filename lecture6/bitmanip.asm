%define EXIT 1
%define READ 3
%define WRITE 4

%define STDIN 0
%define STDOUT 1

[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

buf:    resb 5

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here

        ;; read four bytes of input
        mov eax, READ
        mov ebx, STDIN
        mov ecx, buf
        mov edx, 5              ; expect 4 characters and a newline
        int 80h

        ;; copy into a 32 bit (4 byte) register
        mov eax, [buf]

        ;; remember that the least significant byte appears first in a
        ;; little endian machine so if we read the hex bytes '00 11 22 33'
        ;; then it will look like the number 33221100 if we print eax in gdb
        ;;
        ;; let's convert it to the number 00112233

        mov ecx, eax            ; save a copy of the original value
        shl eax, 24             ; get the 00 into position

        mov ebx, ecx            ; get another copy of the original
        shl ebx, 8              ; put the 11 into position
        and ebx, 0x00FF0000     ; unset everything but the 11
        or eax, ebx             ; set all the bits in eax that are set in ebx

        mov ebx, ecx            ; get another copy of the original
        shr ebx, 8              ; put the 22 into position
        and ebx, 0x0000FF00     ; unset everything but the 22
        or eax, ebx             ; set all the bits in eax that are set in ebx

        shr ecx, 24             ; get the 33 into position (we're done with our copy)
        or eax, ecx             ; no need to mask since everything else is 0 after the shift

        ;; now to prove what we did, copy the result back into memory and print it
        mov [buf], eax

        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, buf
        mov edx, 5              ; the newline is still there so let it be printed
        int 80h

        ;; call sys_exit to finish things off
        mov eax, EXIT           ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
