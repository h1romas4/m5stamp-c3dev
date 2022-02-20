cmake_minimum_required(VERSION 3.5)

set(COMPONENT_ADD_INCLUDEDIRS
    tinyPNG
)

file(GLOB SRCS
    tinyPNG/tinyPNG.cpp
)
set(COMPONENT_SRCS ${SRCS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CXX_COMPILE_OPTIONS "-Dtypeof=__typeof__ -DDEFLATE_ALLOCATION_DYNAMIC")

register_component()
