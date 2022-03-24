cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    agfxlib
    st7735
)

set(SRCS
    agfxlib/Adafruit_GFX.cpp
    agfxlib/Adafruit_SPITFT.cpp
    agfxlib/glcdfont.c
    st7735/Adafruit_ST7735.cpp
    st7735/Adafruit_ST77xx.cpp
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS} REQUIRES arduino-esp32)

target_compile_options(${COMPONENT_TARGET} PRIVATE
    -O3
)
