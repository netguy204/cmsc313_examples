[section .data]
allocName:      db "alloc", 0
initName:       db "init", 0

[section .text]
global call
global supercall
global class_new_object

extern class_find_method
extern method_find_supermethod
extern method_imp
extern header_get_

call:
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




supercall:
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


class_new_object:
        push ebp
        mov ebp, esp

        ;; call alloc on the class
        push dword [ebp + 12]   ; the class
        push allocName          ; "alloc"
        call call
        add esp, 8

        mov dword [ebp + 8], initName ; message: "init"
        mov [ebp + 12], eax           ; the new object

        ;; clean what we did on the stack
        mov esp, ebp
        pop ebp
        jmp call
