cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    M5Unit-Sonic/src
)

set(SRCS
    M5Unit-Sonic/src/Unit_Sonic.cpp
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS} REQUIRES arduino-esp32)
