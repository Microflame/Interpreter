add_executable(bench benchmark.cpp)
target_link_libraries(bench benchmark::benchmark)
target_link_libraries(bench slip)
