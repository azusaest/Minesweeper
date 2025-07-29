cmake_minimum_required(VERSION 3.10)
set( CMAKE_C_COMPILER "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe" )
set( CMAKE_CXX_COMPILER "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe" )
project(Minesweeper)

set(CMAKE_CXX_STANDARD 17)

# Set UTF-8 encoding for source files
if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(Minesweeper main.cpp)

# 對於 Windows GUI 應用程式，需加上 WIN32
if (WIN32)
    set_target_properties(Minesweeper PROPERTIES WIN32_EXECUTABLE YES)
endif() 