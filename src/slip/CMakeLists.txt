file(GLOB SOURCES
    *.hpp
    *.cpp
    util/*.hpp
    util/*.cpp
)

add_library(slip STATIC ${SOURCES})
