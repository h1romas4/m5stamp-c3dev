cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    TinyGPSPlus/src
)

set(SRCS
    TinyGPSPlus/src/TinyGPS++.cpp
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS} REQUIRES arduino-esp32)
