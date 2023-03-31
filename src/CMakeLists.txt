add_subdirectory(slip)
add_subdirectory(benchmark)

add_executable(slip1 main.cpp)
target_link_libraries(slip1 slip)
