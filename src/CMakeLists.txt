cmake_minimum_required (VERSION 3.16)

project(js)

find_package(SQLite3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(js
    main.cpp
)
target_compile_options(js PRIVATE -Wall -Wextra -Werror)
target_link_libraries(js PRIVATE ${SQLite3_LIBRARIES})
