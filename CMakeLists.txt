cmake_minimum_required(VERSION 3.0)

project(Interp VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fno-omit-frame-pointer -g -Wall -pedantic -Wno-unused-function")

include_directories(src)

add_subdirectory(src)
