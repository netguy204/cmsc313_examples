%define STDOUT 1
%define WRITE 4

[SECTION .data]
magicNumber:    db  "0x"
magicNumberLen: EQU $-magicNumber


[SECTION .bss]
        Hex resb 10	; the hex string we want to print out

[SECTION .text]
;;; This is where our program lives.
global prt_hex                  ; make sure the linker can find our routine

prt_hex:                        ; this will be called over and over again by the test driver

        ;; as an example, we protect the register's we're going to use
        ;; and then print out magicNumber. Your code should actually
        ;; print the number that was given to you in EAX in hexadecimal
        mov dword [Hex], 0
        mov dword [Hex + 4], 0
        mov word [Hex + 8], 0
        ror al, 4
        ror ah, 4
        bswap eax
        ror al, 4
        ror ah, 4
        push eax
        push ebx
        push ecx
        push edx
        mov ecx, 0	; establish a counter for which hex character we're on
        mov ebx, 0	; counts non-zero digits
        cmp eax, 0
        je .totalZero

        ; eax is the number we need to convert to hexadecimal
.doDivide:
        mov edx, 0	; DIV divides the 64 bit number EDX:EAX by the operand. Thus we need to set edx to 0
        push ecx	; save value of our counter
        mov ecx, 16
        div ecx		; remainder stored in EDX, quotient(operate on next) is stored in EAX
        pop ecx		; restore our counter
        add ebx, edx	; if all chars have been 0 so far, ebx will now be zero
        cmp ebx, 0
        je .isZero
        inc ebx		; add 1 to ebx because there is atleast 1 non-zero hex char
        cmp edx, 9
        jbe .toNumber	; transform edx into an ascii character for the number it represents
        add edx, 55	; convert remainder into ascii letter A-F
        jmp .addToString
.isZero:
        inc ecx
        jmp .doDivide
.toNumber:
        add edx, '0'	; convert remainder into ascii character 0-9
.addToString:
        mov byte [Hex + ecx + 2], dl	; offset by 2 so we can add in "0x" later
        inc ecx		; increase counter by 1 now, so later we know we're done with this nibble
        cmp ecx, 8
        je .finishAndRestore		; if all 8 nibbles have been processed, finish
        jmp .doDivide
        ; done converting
.finishAndRestore:
        mov word bx, [magicNumber]	; move "0x" into ebx
        mov word [Hex], bx	; move "0x" into the string
        add ebx, 2		; ebx holds num chars to print, add 2 for "0x"
        mov edx, ebx		; do this early
        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, Hex
        int 80h
        ;; restore the registers we manipulated
        pop edx
        pop ecx
        pop ebx
        pop eax

        ;; return
        ret
.totalZero:
        inc ecx
        mov edx, '0'
        mov byte [Hex + 2], dl
        jmp .finishAndRestore
