file(GLOB SOURCES
    *.hpp
    *.cpp
    util/*.hpp
    util/*.cpp
)

add_library(slip STATIC ${SOURCES})
target_link_libraries(slip fmt::fmt)
