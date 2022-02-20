cmake_minimum_required(VERSION 3.5)
set(COMPONENT_ADD_INCLUDEDIRS
    agfxlib
    st7735
)

file(GLOB SRCS
    agfxlib/Adafruit_GFX.cpp
    agfxlib/Adafruit_SPITFT.cpp
    agfxlib/glcdfont.c
    st7735/Adafruit_ST7735.cpp
    st7735/Adafruit_ST77xx.cpp
)
set(COMPONENT_SRCS ${SRCS})
set(COMPONENT_REQUIRES arduino-esp32)

register_component()
