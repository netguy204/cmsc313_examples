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
gcc -std=c99 -g -m32 -o $EXEFILE $*
