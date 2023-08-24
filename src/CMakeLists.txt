cmake_minimum_required (VERSION 3.10)

project(protobuf_example)

# 郭强，2023-7-25

# 本例有俩程序：C++的和C的。前者使用Ubuntu预置的转换工具，后者则依赖于GitHub上clone下来的nanopb。

# 对于C++的例子，需要在Ubuntu上安装包。命令是：
#     sudo apt install libprotobuf-dev protobuf-compiler

# 对于C的例子要麻烦一些：nanopb需要跟本例并行存在于目录下，具体是 本CMakeLists.txt所在的目录的 ../../nanopb下，
# 其共享的工具 和 .c/.h 文件会被本例使用

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)


# host-side PB

#主机端产生PB的源程序的标准套路
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS_HOST PROTO_HDRS_HOST remote_message.proto)

# 生成程序
add_executable(protobuf-example-host
    example-host.cpp

    ${PROTO_SRCS_HOST}
    ${PROTO_HDRS_HOST}
)

#程序依赖的头文件和库。因为pb生成的程序在 build目录中，所以得加进来；PB本身的头文件也加进来
target_include_directories(protobuf-example-host PRIVATE ${Protobuf_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(protobuf-example-host PRIVATE -Wall -Wextra -Werror)
# PB库也加进来
target_link_libraries(protobuf-example-host ${Protobuf_LIBRARIES})


#mcu-side PB
# 再次强调，这个nanopb的总目录需要存在在制定位置
set(NANOPB_REPO_ROOT ../../nanopb)

# nanopb生成的特定消息的编解码程序存在这里。注意一定不能跟host的放在一起，因为 .h文件名是一样的
set(NANOPB_INTERMEDIATE_DIR ${CMAKE_SOURCE_DIR}/../nanopb_intermediate)
set(PROTO_SRCS_MCU ${NANOPB_INTERMEDIATE_DIR}/remote_message.pb.c)
set(PROTO_HDRS_MCU ${NANOPB_INTERMEDIATE_DIR}/remote_message.pb.h)

# 特定消息的编解码程序 得手工生成。指定路径 命令 输入 输出 以及执行命令的时候的当前路径
add_custom_command(
    OUTPUT
        ${PROTO_SRCS_MCU} ${PROTO_HDRS_MCU}
    COMMAND
        ${NANOPB_REPO_ROOT}/generator/protoc --nanopb_out=${NANOPB_INTERMEDIATE_DIR}  remote_message.proto
    DEPENDS
        remote_message.proto remote_message.options
    WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
    COMMENT
        nanopb executing
)

# MCU端的程序
add_executable(protobuf-example-mcu
    example-mcu.c

    #特定消息的c程序
    ${PROTO_SRCS_MCU}

    #针对所有消息的C程序。注意是直接引用了nanopb
    ${NANOPB_REPO_ROOT}/pb_encode.c
    ${NANOPB_REPO_ROOT}/pb_decode.c
    ${NANOPB_REPO_ROOT}/pb_common.c

    # 跟上面4个对应，不过是 .h
    ${PROTO_HDRS_MCU}
    ${NANOPB_REPO_ROOT}/pb_encode.h
    ${NANOPB_REPO_ROOT}/pb_decode.h
    ${NANOPB_REPO_ROOT}/pb.h
)

# 注意不能包含 ${CMAKE_CURRENT_BINARY_DIR}，因为会和C++的输出头文件打架
target_include_directories(protobuf-example-mcu PRIVATE ${NANOPB_REPO_ROOT} ${NANOPB_INTERMEDIATE_DIR})
target_compile_options(protobuf-example-mcu PRIVATE -Wall -Wextra -Werror)

add_executable(datagram datagram_codec.c crc8_soft.c)
target_compile_options(datagram PRIVATE -Wall -Wextra -Werror)
