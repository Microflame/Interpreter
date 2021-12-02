add_subdirectory(common)
add_subdirectory(util)
add_subdirectory(scanner)
add_subdirectory(parser)
# add_subdirectory(interpreter)


add_executable(Interp main.cc es_to_string.cpp)

# target_link_libraries(Interp Common Util Interpreter)
target_link_libraries(Interp Common Util)
