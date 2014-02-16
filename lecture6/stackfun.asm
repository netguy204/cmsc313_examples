%define SYSCALL_WRITE 4
%define SYSCALL_READ 3
%define STDIN 0
%define STDOUT 1


[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

hello:  db "Hello, world!", 10, 0

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

readbuf:        resb 128
readbuflen:     EQU $ - readbuf

callstack:      resd 256
datastack:      resd 256

;;; define a few macros that let us use callstack and datastack much
;;; like we would use the hardware stack via push and pop. Except,
;;; the callstack is only used for calling functions and returning
;;; from them. The datastack is only used for arguments and return
;;; values. The hardware stack is used internally by functions to
;;; ensure that any register modifications made do not escape.

%macro callstack_init 0
        mov esi, callstack
        mov edi, datastack
%endmacro



%macro ccall 1
        mov [esi], dword %%continue
        add esi, dword 4
        jmp %1
%%continue:
%endmacro

%macro creturn 0
        sub esi, 4
        jmp [esi]
%endmacro



%macro dpush 1
        mov [edi], dword %1
        add edi, dword 4
%endmacro

%macro dpop 1
        sub edi, dword 4
        mov %1, dword [edi]
%endmacro

%macro dpop 0
        sub edi, dword 4
%endmacro

%define dtop dword [edi - 4]


%macro prelude 1
        push ebp
        mov ebp, esp
        sub esp, (%1 * 4)
%endmacro

%macro prelude 0
        push ebp
        mov ebp, esp
%endmacro

%macro postlude 0
        mov esp, ebp
        pop ebp
%endmacro

%define temp1 [ebp - 4]
%define temp2 [ebp - 8]
%define temp3 [ebp - 12]

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

strlen:
        push eax
        push ebx

        dpop eax
        mov ebx, 0

.loop:
        cmp byte [eax], 0
        je .loopend

        inc ebx
        inc eax
        jmp .loop

.loopend:
        dpush ebx

        pop ebx
        pop eax

        creturn

printstr:
        push eax
        push ebx
        push ecx
        push edx

        mov eax, SYSCALL_WRITE
        mov ebx, STDOUT
        mov ecx, dtop           ; the string
        ccall strlen
        dpop edx                ; the length
        int 80h

        pop edx
        pop ecx
        pop ebx
        pop eax

        creturn

readstr:
        push eax
        push ebx
        push ecx
        push edx

        mov eax, SYSCALL_READ
        mov ebx, STDIN
        dpop ecx                ; the buffer
        dpop edx                ; buffer size
        int 80h

        dpush eax               ; number of bytes read

        pop edx
        pop ecx
        pop ebx
        pop eax

        creturn

_start:                         ; the program actually starts here
        callstack_init

        dpush readbuflen-1
        dpush readbuf
        ccall readstr
        dpop eax                ; bytes read
        mov [readbuf + eax], byte 0 ; null terminate

        mov eax, 10
.loop:
        and eax, eax
        jz .loopend

        dpush readbuf
        ccall printstr

        dec eax
        jmp .loop

.loopend:
        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
