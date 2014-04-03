[section .text]
global object_call
extern class_find_method

object_call:
        push ebp
        mov ebp, esp

        ;; look up the actual target of this call
        push dword [ebp + 8]    ; the name
        mov eax, [ebp + 12]     ; the object
        push dword [eax]        ; the class
        call class_find_method
        add esp, 8

        mov [ebp + 8], eax      ; replace the name with the method
        mov eax, [eax]          ; get the implementation

        ;; now tweak the stack so that the implementation gets all of the arguments
        ;; it needs.
        mov esp, ebp
        pop ebp
        jmp eax
