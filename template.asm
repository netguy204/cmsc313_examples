[SECTION .data]
;;; Here we declare initialized data. For example: messages, prompts,
;;; and numbers that we know in advance

[SECTION .bss]
;;; Here we declare uninitialized data. We're reserving space (and
;;; potentially associating names with that space) that our code
;;; will use as it executes. Think of these as "global variables"

[SECTION .text]
;;; This is where are program lives.
extern start                    ; make start global so ld can find it

start:                          ; the program actually starts here
