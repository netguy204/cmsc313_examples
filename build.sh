#!/bin/bash

if [ "$1" == "" ]; then
    echo "usage: $0 input.asm"
    exit 1
fi

INFILE=$1
NO_EXTENSION="${INFILE%.*}"
OBJFILE="${NO_EXTENSION}.o"
EXEFILE="${NO_EXTENSION}"

# force 32 bit code generation
nasm -f elf -g -F stabs $INFILE
ld -o $EXEFILE $OBJFILE -melf_i386
