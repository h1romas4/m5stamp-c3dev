cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    Adafruit_NeoPixel
)

set(SRCS
    Adafruit_NeoPixel/esp.c
    Adafruit_NeoPixel/Adafruit_NeoPixel.cpp
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS} REQUIRES arduino-esp32)

target_compile_options(${COMPONENT_TARGET} PRIVATE
    -O3
)
