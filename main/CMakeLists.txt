set(INCLUDEDIRS
    ./
)

set(SRCS
    main.cpp
    test_wasm3.cpp
    test_freetype.cpp
    test_tinypng.cpp
    test_i2c_gpio1819.cpp
)

idf_component_register(
    INCLUDE_DIRS ${INCLUDEDIRS}
    SRCS ${SRCS}
<<<<<<< HEAD
    REQUIRES arduino-esp32 freetype lcd tinypng unitenv wasm)
=======
    REQUIRES arduino-esp32 freetype lcd tinypng wasm unitenv)
>>>>>>> c14df03664148bc21c1646d222e269daee480abf
