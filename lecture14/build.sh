#!/bin/bash

NASM="nasm -f elf -g -F stabs"
GCC="gcc -m32"

$GCC -c calling_c.c -o calling_c.c.o
$NASM calling_c.asm -o calling_c.asm.o
$GCC -o calling_c calling_c.c.o calling_c.asm.o
