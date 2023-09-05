# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("serialport")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)
add_library( # Sets the name of the library.
        serialport

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        SerialPort.c
        ../mcu/host-mcu-common/source/serial_datagram.c
        ../mcu/mcu-driver/source/uart_io_API.c
        ../mcu/pb_intermediate/src/pb_common.c
        ../mcu/pb_intermediate/src/pb_decode.c
        ../mcu/pb_intermediate/src/pb_encode.c
        ../mcu/host-mcu-common/source/datagram_codec.c
        ../mcu/host-mcu-common/source/crc8_soft.c
        ../mcu/pb_intermediate/src/remote_message.pb.c
        ../mcu/semantic_api/semantic_api.c
        ../mcu/pb_intermediate/src/semantic_api.generated.c)

include_directories("../mcu/MH2101_SDK/Libraries/Device/MegaHunt/mhscpu/Include")
include_directories("../mcu/driver/include")
include_directories("../mcu/host-mcu-common/include")
include_directories("../mcu/pb_intermediate/include")
include_directories("../mcu/semantic_api")
include_directories("../mcu/MH2101_SDK/Libraries/CMSIS/Include")
# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        serialport
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})