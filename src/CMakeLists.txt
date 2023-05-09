cmake_minimum_required (VERSION 3.16)

project(js)

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS txt_num.proto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(js
    main.cpp
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

include_directories(${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(js PRIVATE -Wall -Wextra -Werror)
target_link_libraries(js ${Protobuf_LIBRARIES})
