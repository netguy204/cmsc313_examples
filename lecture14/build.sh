#!/bin/bash

NASM="nasm -f elf -g -F stabs"
GCC="gcc -m32"
LD="ld -melf_i386"

function echodo {
    echo $*
    $*
}

echo "building the calling_c example"
echodo $GCC -c calling_c.c -o calling_c.c.o
echodo $NASM calling_c.asm -o calling_c.asm.o
echodo $LD -o calling_c calling_c.c.o calling_c.asm.o
echo

echo "building the calling_asm example"
echodo $GCC -c calling_asm.c -o calling_asm.c.o
echodo $NASM calling_asm.asm -o calling_asm.asm.o
echodo $GCC -o calling_asm calling_asm.asm.o calling_asm.c.o
