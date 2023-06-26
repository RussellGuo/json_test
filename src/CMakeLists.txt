cmake_minimum_required (VERSION 3.16)

project(js)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(js
    main.cpp
)

set(ARM_LOCAL_USR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../local)
include_directories(${ARM_LOCAL_USR_DIR}/include)

target_compile_options(js PRIVATE -Wall -Wextra -Werror)
target_link_libraries(js ${ARM_LOCAL_USR_DIR}/lib/libcrc32c.so ${ARM_LOCAL_USR_DIR}/lib/libssl.a ${ARM_LOCAL_USR_DIR}/lib/libcrypto.a)
