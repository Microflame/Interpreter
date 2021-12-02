file(GLOB SOURCES
    *.h
    *.cc
    util/*.h
    util/*.cc
)

add_executable(Interp ${SOURCES})