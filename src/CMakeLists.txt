cmake_minimum_required (VERSION 3.16)

project(sign_firmware_pc)

find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(sign_firmware_pc
    main.cpp
    rsa_sha_demo.c
)

target_compile_options(sign_firmware_pc PRIVATE -Wall -Wextra -Werror)
target_link_libraries(sign_firmware_pc OpenSSL::SSL)
