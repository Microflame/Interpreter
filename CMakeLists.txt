cmake_minimum_required(VERSION 3.0)

project(Interp VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror")

include_directories(src)

add_subdirectory(src)
