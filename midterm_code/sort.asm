;;; Context: This is implements the final stage of a merge-sort.
;;; inputA and inputB are in order internally and this program
;;; combines them into a final sorted list.
;;;
;;; You are responsible for answering the questions in the comments
;;; marked with Q#
;;;
;;; For bonus points:
;;;
;;; Write an algorithm in C-like syntax that accomplishes the same
;;; thing as this body of code by implementing the function:
;;;
;;; void mergeLists(char* result, char* inputA, char* inputB)
;;;
%define WRITE 4
%define STDOUT 1
%define NEWLINE 10

[SECTION .data]

inputA:         db "abcqrz"
inputALen:      equ $-inputA

inputB:         db "efglmnwxy"
inputBLen:      equ $-inputB

[SECTION .bss]

resultLen:      equ (inputALen + inputBLen + 1)
result:         resb (inputALen + inputBLen + 1)

[SECTION .text]
global _start

_start:
        mov edi, result         ; Q1: What is EDI used to represent throughout this program?
        mov eax, 0              ; EAX is the current index into inputA
        mov ebx, 0              ; EBX is the current index into inputB

.loop:
        cmp eax, inputALen      ; Q2: What condition in the algorithm is being checked for?
        je .finishB             ; Q2: Is it triggered for the provided input?

        cmp ebx, inputBLen      ; Q3: What condition in the algorithm is being checked for?
        je .finishA             ; Q3: Is it triggered for the provided input?

        mov dl, [inputA + eax]  ; Grab the next character in inputA
        mov dh, [inputB + ebx]  ; and the next character in inputB

        cmp dl, dh              ; Q4: What if statement would be equivalent to these
        jle .nextFromA          ; Q4: two lines of code?

        ;; get the next character from B
        mov [edi], dh
        inc ebx                 ; Q5: Why is EBX incremented?
        jmp .finishLoopBody

.nextFromA:
        mov [edi], dl
        inc eax                 ; Q6: Why is EAX incremented?

.finishLoopBody:
        inc edi                 ; Q7: Why is EDI incremented?
        jmp .loop

.finishA:
        mov esi, inputA
        mov ecx, inputALen
        jmp .finishCopy

.finishB:
        mov esi, inputB
        mov eax, ebx
        mov ecx, inputBLen
        jmp .finishCopy         ; Q8: Is this line required? Why or why not?

.finishCopy:                    ; Q9: How is this loop able to handle both the finishA
        cmp eax, ecx            ; Q9: and the finishB case?
        je .end

        mov dl, [esi + eax]
        mov [edi], dl
        inc eax
        jmp .finishCopy

.end:
        ;; append a newline
        mov [result + resultLen - 1], byte NEWLINE      ; Q10: Is the address arithmetic inside the brackets evaluated
                                                        ; Q10: in advance by NASM or is it computed when when your
                                                        ; Q10: program runs?

        mov eax, WRITE
        mov ebx, STDOUT
        mov ecx, result
        mov edx, resultLen
        int 80h                                 ; Q11: What exact text will appear on the terminal as
                                                ; Q11: a result of this sytem call?

        ;; call sys_exit to finish things off
        mov eax, 1              ; sys_exit syscall
        mov ebx, 0              ; no error
        int 80H                 ; kernel interrupt
