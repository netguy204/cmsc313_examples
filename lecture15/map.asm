[section .text]

global mapasm
extern array_alloc

mapasm:
        push ebp
        mov ebp, esp

        ;; ebx, edi, and esi are the registers we use locally. these were
        ;; chosen because they're callee protected so we won't have
        ;; to push them before every call to the user's function

        push ebx                ; protect, we use this as the array ptr
        push esi                ; protect, use this as the count
        push edi                ; protect, the destination array

        mov ebx, [ebp + 8]      ; get the sized array
        mov esi, [ebx]          ; the size is at the start of the struct
        add ebx, 4              ; then the data starts

        push esi                ; allocate our destination array
        call array_alloc
        add esp, 4              ; pop the argument
        mov edi, eax            ; and store away the return value

.loop:
        cmp esi, 0              ; still data to process?
        je .end

        dec esi                         ; esi starts at one past the last element
        push dword [ebx + esi * 4]      ; push that value (each element is 4 bytes)
        mov eax, [ebp + 12]             ; get the function we were passed
        call eax                        ; call it
        mov [edi + esi * 4 + 4], eax    ; store the result (+4 becase edi points to the start of a sizedarray)
        add esp, 4                      ; pop the argument

        jmp .loop

.end:
        mov eax, edi            ; the return value

        pop edi
        pop esi
        pop ebx
        mov esp, ebp
        pop ebp

        ret
