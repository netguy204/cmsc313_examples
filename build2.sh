#!/bin/bash

if [ "$1" == "" ]; then
    echo "usage: $0 input.asm"
    exit 1
fi

INFILE=$1
shift

NO_EXTENSION="${INFILE%.*}"
OBJFILE="${NO_EXTENSION}.o"
EXEFILE="${NO_EXTENSION}"

# force 32 bit code generation
nasm -f elf -g -F stabs $INFILE
gcc -o $EXEFILE $OBJFILE -m32 -L/usr/lib -lc $*
