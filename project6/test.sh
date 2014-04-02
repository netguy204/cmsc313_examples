#!/bin/bash

function difftest() {
    echo "Comparing $1 and $2"
    echo "============"

    diff $1 $2

    if [ "$?" == "0" ]; then
        echo "============"
        echo "Program output matches expected"
    else
        echo "============"
        echo "Program output does not match expected"
    fi
    echo
}

./test1.bin > test1.output
./test2.bin > test2.output
./test3.bin ticks.txt > test3.output

difftest test1.output test1.expected
difftest test2.output test2.expected
difftest test3.output test3.expected
