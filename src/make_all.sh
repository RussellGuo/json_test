#!/bin/bash

echo to gen mata code tools...
g++ -O3 -o gen_code_from_json main.cpp

echo to gen mata code...
./gen_code_from_json 2038json.json > 2038json.inc

echo to gen test json program...
g++ -O3 -Wall -Wextra -o x86-test-json target_main.cpp

echo to test json...
./x86-test-json

echo to gen test json program w/o exceptions...
g++ -fno-exceptions -O3 -Wall -Wextra -o x86-test-json-ne target_main.cpp

echo to test json w/o exceptions...
./x86-test-json-ne

CROSS_GPP=aarch64-linux-gnu-g++-10

echo to gen target test json programs...
$CROSS_GPP -Wall -Wextra                                                                  -o arm-test-json-0 target_main.cpp
$CROSS_GPP -Wall -Wextra -mfloat-abi=softfp -mfpu=vfp                                     -o arm-test-json-1 target_main.cpp
$CROSS_GPP -Wall -Wextra -mfloat-abi=softfp -mfpu=neon                                    -o arm-test-json-2 target_main.cpp
$CROSS_GPP -Wall -Wextra -mcpu=cortex-a8 -march=armv7-a -mfloat-abi=softfp -mfpu=vfp      -o arm-test-json-3 target_main.cpp
$CROSS_GPP -Wall -Wextra -mcpu=cortex-a8 -march=armv7-a -mfloat-abi=softfp -mfpu=neon     -o arm-test-json-4 target_main.cpp

$CROSS_GPP -O3 -Wall -Wextra                                                              -o arm-test-json-5 target_main.cpp
$CROSS_GPP -O3 -Wall -Wextra -mfloat-abi=softfp -mfpu=vfp                                 -o arm-test-json-6 target_main.cpp
$CROSS_GPP -O3 -Wall -Wextra -mfloat-abi=softfp -mfpu=neon                                -o arm-test-json-7 target_main.cpp
$CROSS_GPP -O3 -Wall -Wextra -mcpu=cortex-a8 -march=armv7-a -mfloat-abi=softfp -mfpu=vfp  -o arm-test-json-8 target_main.cpp
$CROSS_GPP -O3 -Wall -Wextra -mcpu=cortex-a8 -march=armv7-a -mfloat-abi=softfp -mfpu=neon -o arm-test-json-9 target_main.cpp

for f in arm-test-json-*; do
    aarch64-linux-gnu-strip -s $f
done

echo All done!
