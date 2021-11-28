cmake_minimum_required(VERSION 3.0)

project(Interp VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -pedantic -O0")

include_directories(src)

add_subdirectory(src)
