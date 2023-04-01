cmake_minimum_required(VERSION 3.0)

project(Slip VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fno-omit-frame-pointer -g -Wall -pedantic -Wno-unused-function")

find_package(benchmark REQUIRED)

include_directories(src)

add_subdirectory(src)

add_subdirectory(third_party/fmt EXCLUDE_FROM_ALL)
