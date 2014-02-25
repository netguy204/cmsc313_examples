%define STDOUT 1
%define WRITE 4

[SECTION .data]
plusStr:        db " plus "
plusStrLen:     EQU $-plusStr

equStr:         db " equals "
equStrLen:      EQU $-equStr

newline:        db `\n`

[SECTION .bss]


[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it
extern prt_hex

prt_newline:
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

a_plus_b:
        push eax
        push ebx

        ;; num
        call prt_hex

        ;; " plus "
        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, plusStr
        mov edx, plusStrLen
        int 80h

        ;; num
        mov eax, [esp]
        call prt_hex

        ;; " equals "
        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, equStr
        mov edx, equStrLen
        int 80h

        ;; num + num
        pop ebx
        pop eax
        add eax, ebx
        call prt_hex

        ret

_start:                         ; the program actually starts here

        ;; a couple of representative cases to test num->hex conversion
        mov eax, 0
        call prt_hex
        call prt_newline

        mov eax, 0xffff
        call prt_hex
        call prt_newline

        mov eax, 0xffffffff
        call prt_hex
        call prt_newline

        mov eax, 42
        call prt_hex
        call prt_newline

        mov eax, 25
        mov ebx, 50
        call a_plus_b
        call prt_newline

        mov eax, 18
        mov ebx, 2
        call a_plus_b
        call prt_newline

        ;; verify that registers are protected
        mov eax, 41
        mov ebx, 40
        call prt_hex
        call prt_newline

        mov eax, ebx
        call prt_hex
        call prt_newline

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
