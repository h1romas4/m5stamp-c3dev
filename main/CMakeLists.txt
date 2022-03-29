set(INCLUDEDIRS
    ./
)

set(SRCS
    main.cpp
    test_wasm3.cpp
    test_freetype.cpp
    test_tinypng.cpp
    test_nvs_wifi.cpp
    test_i2c_gpio1819.cpp
)

idf_component_register(
    INCLUDE_DIRS ${INCLUDEDIRS}
    SRCS ${SRCS}
    REQUIRES arduino-esp32 freetype lcd tinypng wasm unitenv)
