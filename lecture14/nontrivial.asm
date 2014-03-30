[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]

global sumsqrs

;;; computes sum(x^2, 0, n)
sumsqrs:
        ;; protect callers ebp so we can use it
        push ebp

        ;; remember the "initial" position of the stack. we'll use this
        ;; as a reference point for finding our local variables
        mov ebp, esp

        ;; reserve space for our 32 bit counter and our sum
        sub esp, 8

        ;; initialize the count and sum
        mov eax, [ebp + 8]      ; the requested N
        mov [ebp - 4], eax      ; set count to start there

        mov [ebp - 8], dword 0  ; the initial sum

.loop:
        cmp [ebp - 4], dword 0  ; terminate when counter is zero
        je .end

        mov eax, [ebp - 4]      ; next x
        imul eax                ; squared
        add [ebp - 8], eax      ; added to result

        dec dword [ebp - 4]     ; decrement count
        jmp .loop

.end:
        mov eax, [ebp - 8]      ; result goes into eax
        mov esp, ebp            ; restore the callers stack pointer
        pop ebp                 ; and base pointer
        ret                     ; then return
