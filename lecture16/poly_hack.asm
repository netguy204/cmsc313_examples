;;; Build note:
;;;
;;; The macro C() must be defined in the call to Nasm. This allows us to
;;; switch the name mangling rules at build time.

[section .data]
allocName:      db "alloc", 0
initName:       db "init", 0

[section .text]
global C(invoke)
global C(superinvoke)
global C(class_new_object)

extern C(class_find_method)
extern C(method_supermethod)
extern C(method_imp)
extern C(header_get_)


;;; Exploit the CDECL calling convention and varargs to invoke our target
;;; by manipulating the arguments we were given and then jmp'ing so that
;;; it gets all the arguments we had and will return to our caller instead
;;; of to us. This is called a "tailcall" because invoke doesn't grow the
;;; stack
C(invoke):
        push ebp
        mov ebp, esp

        sub esp, 12              ; alignment

        ;; look up the actual target of this invoke
        push dword [ebp + 8]    ; the name

        ;; convert the object into a class
        push dword [ebp + 12]   ; the object
        call C(header_get_)     ; the header

        mov eax, [eax]          ; the class
        mov [esp], eax

        push dword 0            ; the method (not used)
        call C(class_find_method)
        add esp, 24

        mov [ebp + 8], eax      ; replace the name with the method

        push eax                ; get the implementation from the method
        push dword 0            ; the method (not used)
        call C(method_imp)
        add esp, 8

        ;; now tweak the stack so that the implementation gets all of the arguments
        ;; it needs.
        mov esp, ebp
        pop ebp
        jmp eax

;;; Invoke "alloc" and then tail-invoke "init" on the result. Since invoke will
;;; tailcall into init the init method will return to our caller instead of to
;;; us.
C(class_new_object):
        push ebp
        mov ebp, esp

        ;; invoke alloc on the class
        push dword [ebp + 12]   ; the class
        push allocName          ; "alloc"
        call C(invoke)
        add esp, 8

        mov dword [ebp + 8], initName ; message: "init"
        mov [ebp + 12], eax           ; the new object

        ;; clean what we did on the stack
        mov esp, ebp
        pop ebp
        jmp C(invoke)
