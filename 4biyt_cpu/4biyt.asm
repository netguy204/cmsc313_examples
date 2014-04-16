;;; count to 5 and then recount
start:
        mov a, loop             ; set the target of future jg instructions
loop:
        add d, 1                ; increment d
        jg 5, d                 ; jump if 5 is greater than d. target is location in a
        mov d, 0                ; reset d
        jmp start               ; start over again
