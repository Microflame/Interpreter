add_executable(bench_math bench_math.cpp)
target_link_libraries(bench_math benchmark::benchmark slip)

add_executable(bench_string_format bench_string_format.cpp)
target_link_libraries(bench_string_format benchmark::benchmark slip fmt::fmt)
