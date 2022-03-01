cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    UNIT_ENV/src
)

set(SRCS
    UNIT_ENV/src/DHT12.cpp
    UNIT_ENV/src/QMP6988.cpp
    UNIT_ENV/src/SHT3X.cpp
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS} REQUIRES arduino-esp32)
