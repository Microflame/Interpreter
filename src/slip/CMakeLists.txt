set(SOURCES
    context.cpp
    expr.cpp
    object.cpp
    parser.cpp
    source.cpp
    stmt.cpp
    token_type.cpp
    token.cpp
    tokenizer.cpp

    util/es_to_string.cpp
    util/math.cpp
    util/string_tools.cpp
)

add_library(slip STATIC ${SOURCES})
target_link_libraries(slip fmt::fmt)
