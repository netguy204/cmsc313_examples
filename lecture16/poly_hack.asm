[section .text]
global object_call
global object_supercall

extern class_find_method
extern method_find_supermethod
extern method_imp
extern header_get_

object_call:
        push ebp
        mov ebp, esp

        ;; look up the actual target of this call
        push dword [ebp + 8]    ; the name

        ;; convert the object into a class
        push dword [ebp + 12]   ; the object
        call header_get_        ; the header

        mov eax, [eax]          ; the class
        mov [esp], eax

        push dword 0            ; the method (not used)
        call class_find_method
        add esp, 12

        mov [ebp + 8], eax      ; replace the name with the method

        push eax                ; get the implementation from the method
        call method_imp
        add esp, 4

        ;; now tweak the stack so that the implementation gets all of the arguments
        ;; it needs.
        mov esp, ebp
        pop ebp
        jmp eax




object_supercall:
        push ebp
        mov ebp, esp

        ;; look up the actual target of this call
        push dword [ebp + 8]    ; the previous method
        push dword 0            ; the method (not used)
        call method_find_supermethod
        add esp, 8

        mov [ebp + 8], eax      ; replace the previous method with the new method

        push eax                ; get the implementation from the method
        call method_imp
        add esp, 4

        ;; now tweak the stack so that the implementation gets all of the arguments
        ;; it needs.
        mov esp, ebp
        pop ebp
        jmp eax
