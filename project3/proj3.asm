%define STDOUT 1
%define WRITE 4

[SECTION .data]
magicNumber:    db  "0x42"
magicNumberLen: EQU $-magicNumber


[SECTION .bss]


[SECTION .text]
;;; This is where our program lives.
global prt_hex                  ; make sure the linker can find our routine

prt_hex:                        ; this will be called over and over again by the test driver

        ;; as an example, we protect the register's we're going to use
        ;; and then print out magicNumber. Your code should actually
        ;; print the number that was given to you in EAX in hexadecimal

        ;; protect the registers we manipulate
        push eax
        push ebx
        push ecx
        push edx

        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, magicNumber
        mov edx, magicNumberLen
        int 80h

        ;; restore the registers we manipulated
        pop edx
        pop ecx
        pop ebx
        pop eax

        ;; return
        ret
