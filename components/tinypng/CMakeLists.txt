cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    tinyPNG
)

set(SRCS
    tinyPNG/tinyPNG.cpp
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS})

target_compile_options(${COMPONENT_TARGET} PUBLIC
    -fexceptions
    -std=c++17
    -Dtypeof=__typeof__
    -DDEFLATE_ALLOCATION_DYNAMIC)
