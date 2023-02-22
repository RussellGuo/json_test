cmake_minimum_required (VERSION 3.15)

project(js_test)

set(source
    main.cpp
    sub.cpp
)

add_executable(js ${source})
