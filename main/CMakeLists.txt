set(INCLUDEDIRS
    ./
)

set(SRCS
    main.cpp
    test_wasm3_clockenv.cpp
    test_wasm3_gpsgsv.cpp
    test_wasm3_imu6886.cpp
    test_freetype.cpp
    test_tinypng.cpp
    test_nvs_wifi.cpp
    test_i2c_gpio1819.cpp
    test_uart_gpio1819.cpp
)

idf_component_register(
    INCLUDE_DIRS ${INCLUDEDIRS}
    SRCS ${SRCS}
    REQUIRES arduino-esp32 freetype lcd led tinypng lwgps wasm unitenv ultrasonic)
