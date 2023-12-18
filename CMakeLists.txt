cmake_minimum_required(VERSION 3.8)
project(skip_use)

set(CMAKE_CXX_STANDARD 11)

if (MSVC)
    add_compile_options(/source-charset:utf-8)
    add_compile_options(/EHsc)
endif()

add_executable(skip_use main.cpp MSkipList.hpp)
add_executable(skip_test stress.cpp MSkipList.hpp)
