cmake_minimum_required (VERSION 3.10)

project(protobuf_example)

find_package(Protobuf REQUIRED)

# host-side PB
protobuf_generate_cpp(PROTO_SRCS_HOST PROTO_HDRS_HOST txt_num.proto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(protobuf-example-host
    example-host.cpp
    ${PROTO_SRCS_HOST}
    ${PROTO_HDRS_HOST}
)

target_include_directories(protobuf-example-host PRIVATE ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(protobuf-example-host PRIVATE -Wall -Wextra -Werror)
target_link_libraries(protobuf-example-host ${Protobuf_LIBRARIES})

#mcu-side PB

set(NANOPB_REPO_ROOT ../../nanopb)

set(NANOPB_INTERMEDIATE_DIR ${CMAKE_SOURCE_DIR}/../nanopb_intermediate)
set(PROTO_SRCS_MCU ${NANOPB_INTERMEDIATE_DIR}/txt_num.pb.c)
set(PROTO_HDRS_MCU ${NANOPB_INTERMEDIATE_DIR}/txt_num.pb.h)


add_custom_command(
    OUTPUT
        ${PROTO_SRCS_MCU} ${PROTO_HDRS_MCU}
    COMMAND
        ${NANOPB_REPO_ROOT}/generator/protoc --nanopb_out=${NANOPB_INTERMEDIATE_DIR}  txt_num.proto
    DEPENDS
        txt_num.proto txt_num.options
    WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
    COMMENT
        nanopb executing
)

add_executable(protobuf-example-mcu
    example-mcu.c

    ${PROTO_SRCS_MCU}

    ${NANOPB_REPO_ROOT}/pb_encode.c
    ${NANOPB_REPO_ROOT}/pb_decode.c
    ${NANOPB_REPO_ROOT}/pb_common.c

    ${PROTO_HDRS_MCU}
    ${NANOPB_REPO_ROOT}/pb_encode.h
    ${NANOPB_REPO_ROOT}/pb_decode.h
    ${NANOPB_REPO_ROOT}/pb.h
)

target_include_directories(protobuf-example-mcu PRIVATE ${NANOPB_REPO_ROOT} ${NANOPB_INTERMEDIATE_DIR})
target_compile_options(protobuf-example-mcu PRIVATE -Wall -Wextra -Werror)
