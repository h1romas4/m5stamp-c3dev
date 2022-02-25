set(INCLUDEDIRS
    ./
)

set(SRCS
    main.cpp
    test_wasm3.cpp
)

idf_component_register(
    INCLUDE_DIRS ${INCLUDEDIRS}
    SRCS ${SRCS}
    REQUIRES arduino-esp32 freetype lcd tinypng wasm)
