cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    M5EPD/src
    M5EPD/src/base
    M5EPD/src/freetype
    M5EPD/src/sfnt
    M5EPD/src/smooth
    M5EPD/src/truetype
    src/
)

set(SRCS
    M5EPD/src/font_render.c
    M5EPD/src/ffsupport.cpp
    M5EPD/src/truetype/ttinterp.c
    M5EPD/src/truetype/ttobjs.c
    M5EPD/src/truetype/ttgload.c
    M5EPD/src/truetype/truetype.c
    M5EPD/src/truetype/ttsubpix.c
    M5EPD/src/truetype/ttdriver.c
    M5EPD/src/truetype/ttpload.c
    M5EPD/src/truetype/ttpic.c
    M5EPD/src/base/ftoutln.c
    M5EPD/src/base/ftbbox.c
    M5EPD/src/base/ftsnames.c
    M5EPD/src/base/ftsystem.cpp
    M5EPD/src/base/ftpic.c
    M5EPD/src/base/ftgxval.c
    M5EPD/src/base/ftfstype.c
    M5EPD/src/base/ftobjs.c
    M5EPD/src/base/ftgasp.c
    M5EPD/src/base/fttrigon.c
    M5EPD/src/base/basepic.c
    M5EPD/src/base/ftbase.c
    M5EPD/src/base/ftcalc.c
    M5EPD/src/base/ftgloadr.c
    M5EPD/src/base/ftstream.c
    M5EPD/src/base/ftapi.c
    M5EPD/src/base/ftinit.c
    M5EPD/src/base/ftadvanc.c
    M5EPD/src/base/ftbitmap.c
    M5EPD/src/base/ftglyph.c
    M5EPD/src/base/md5.c
    M5EPD/src/base/ftrfork.c
    M5EPD/src/base/ftutil.c
    M5EPD/src/base/ftdebug.c
    M5EPD/src/base/ftdbgmem.c
    M5EPD/src/sfnt/ttsbit0.c
    M5EPD/src/sfnt/sfnt.c
    M5EPD/src/sfnt/ttkern.c
    M5EPD/src/sfnt/ttpost.c
    M5EPD/src/sfnt/ttcmap.c
    M5EPD/src/sfnt/ttbdf.c
    M5EPD/src/sfnt/sfntpic.c
    M5EPD/src/sfnt/sfobjs.c
    M5EPD/src/sfnt/ttsbit.c
    M5EPD/src/sfnt/ttmtx.c
    M5EPD/src/sfnt/ttload.c
    M5EPD/src/sfnt/sfdriver.c
    M5EPD/src/smooth/ftgrays.c
    M5EPD/src/smooth/smooth.c
    M5EPD/src/smooth/ftsmooth.c
    M5EPD/src/smooth/ftspic.c
    src/freetype_helper.c
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS} REQUIRES arduino-esp32)

target_compile_options(${COMPONENT_TARGET} PRIVATE
    -O3
)
