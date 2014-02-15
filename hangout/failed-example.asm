;;; In Hangout 1 I tried to create an example that prevent NASM from
;;; being able to automatically determine the size of operands and
;;; thus force us to use size specifiers. The example I came up with
;;; didn't work. Here's why:

[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

array:  db 1, 2, 3, 4

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where our program lives.
global _start                   ; make start global so ld can find it

_start:                         ; the program actually starts here
        ;; this has no size specifiers so NASM will complain that it
        ;; doesn't know how much it should move from the source to
        ;; the destination
        mov [array + 1], [array]

        ;; this does have a specifier but NASM still complains. Not
        ;; sure why since the size of the operation is no longer
        ;; ambiguous.
        mov [array + 1], byte [array]

        ;; this gets NASM to stop complaining about ambiguous sizes
        ;; but brings us to a new error: "invalid combination of
        ;; opcode and operands"
        mov byte [array + 1], byte [array]

        ;; The reason is that there actually is no opcode for a mov
        ;; with a source and destination that are both in memory (not
        ;; registers). To verify, check out the intel instruction set
        ;; manual here: http://intel.ly/1bdsM9l
        ;;
        ;; So, NASM is saying that while it understands what we want
        ;; to do it simply isn't possible to generate an opcode to do
        ;; that.


        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
