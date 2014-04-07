[section .text]

global addsqrs

addsqrs:
        push ebp
        mov ebp, esp
        sub esp, 4

        mov eax, [ebp + 8] ; (a)
        mov edx, 0
        imul eax
        mov [ebp - 4], eax

        mov eax, [ebp + 12] ; (b)
        imul eax
        add eax, [ebp - 4]

        mov esp, ebp
        pop ebp
        ret
