cmake_minimum_required (VERSION 3.16)

project(sign_firmware_pc)

# 在Ubuntu下，需要用 sudo apt install libssl-dev来确保OpenSSL的需求得到满足
find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(sign_firmware_pc
    main.cpp
    gen_signature.hpp
    firmware_layout_constant.h
)

target_compile_options(sign_firmware_pc PRIVATE -Wall -Wextra)
target_link_libraries(sign_firmware_pc OpenSSL::SSL)

add_executable(t t-rsa.c)
target_compile_options(t PRIVATE -Wall -Wextra)
target_link_libraries(t OpenSSL::SSL)
