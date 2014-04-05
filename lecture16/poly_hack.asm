;;; Build note:
;;;
;;; The macro C() must be defined in the call to Nasm. This allows us to
;;; switch the name mangling rules at build time.

[section .data]
allocName:      db "alloc", 0
initName:       db "init", 0

[section .text]
global _invoke
global _superinvoke
global _class_new_object

extern _class_find_method
extern _method_find_supermethod
extern _method_imp
extern _header_get_

_invoke:
        push ebp
        mov ebp, esp

        sub esp, 12              ; alignment

        ;; look up the actual target of this invoke
        push dword [ebp + 8]    ; the name

        ;; convert the object into a class
        push dword [ebp + 12]   ; the object
        call _header_get_        ; the header

        mov eax, [eax]          ; the class
        mov [esp], eax

        push dword 0            ; the method (not used)
        call _class_find_method
        add esp, 24

        mov [ebp + 8], eax      ; replace the name with the method

        push eax                ; get the implementation from the method
        call _method_imp
        add esp, 4

        ;; now tweak the stack so that the implementation gets all of the arguments
        ;; it needs.
        mov esp, ebp
        pop ebp
        jmp eax




_superinvoke:
        push ebp
        mov ebp, esp

        ;; look up the actual target of this invoke
        push dword [ebp + 8]    ; the previous method
        push dword 0            ; the method (not used)
        call _method_find_supermethod
        add esp, 8

        mov [ebp + 8], eax      ; replace the previous method with the new method

        push eax                ; get the implementation from the method
        call _method_imp
        add esp, 4

        ;; now tweak the stack so that the implementation gets all of the arguments
        ;; it needs.
        mov esp, ebp
        pop ebp
        jmp eax


_class_new_object:
        push ebp
        mov ebp, esp

        ;; invoke alloc on the class
        push dword [ebp + 12]   ; the class
        push allocName          ; "alloc"
        call _invoke
        add esp, 8

        mov dword [ebp + 8], initName ; message: "init"
        mov [ebp + 12], eax           ; the new object

        ;; clean what we did on the stack
        mov esp, ebp
        pop ebp
        jmp _invoke
