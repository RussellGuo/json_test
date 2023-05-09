#!/bin/bash

g++ -O3 -Wall -Wextra -o x86-json-strcat main.cpp
CROSS_GPP=aarch64-linux-gnu-g++-10

$CROSS_GPP -O3 -Wall -Wextra -o arm-json-strcat main.cpp

echo All done!
