cmake_minimum_required (VERSION 3.10)

project(protobuf_example)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS txt_num.proto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(protobuf-example-host
    example-host.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

target_include_directories(protobuf-example-host PRIVATE ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(protobuf-example-host PRIVATE -Wall -Wextra -Werror)
target_link_libraries(protobuf-example-host ${Protobuf_LIBRARIES})
