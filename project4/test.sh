#!/bin/bash

./proj4 > output.txt
diff expected.txt output.txt

if [ "$?" == "0" ]; then
    echo "============"
    echo "Program output matches expected"
    exit 0
else
    echo "============"
    echo "Program output does not match expected"
    exit 1
fi
