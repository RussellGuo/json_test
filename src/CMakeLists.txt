cmake_minimum_required (VERSION 3.16)

project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(udp-time
    main.cpp
)

#include_directories()
target_compile_options(udp-time PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -Warray-bounds -Wconversion)
