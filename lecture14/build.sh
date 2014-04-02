#!/bin/bash

NASM="nasm -f elf -g -F stabs"
GCC="gcc -m32 -g"
LD="ld -melf_i386"

function echodo {
    echo $*
    $*
}

echo "building the calling_c example"
echodo $GCC -c calling_c.c -o calling_c.c.o
echodo $NASM calling_c.asm -o calling_c.asm.o
echodo $LD -o calling_c calling_c.c.o calling_c.asm.o

echo "*Building the calling_asm example*"
echodo $GCC -c calling_asm.c -o calling_asm.c.o
echodo $NASM calling_asm.asm -o calling_asm.asm.o
echodo $GCC -o calling_asm calling_asm.asm.o calling_asm.c.o

echo "*Building the nontrivial example*"
echodo $GCC -c nontrivial.c -o nontrivial.c.o
echodo $NASM nontrivial.asm -o nontrivial.asm.o
echodo $GCC -o nontrivial nontrivial.asm.o nontrivial.c.o
