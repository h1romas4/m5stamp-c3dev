set(COMPONENT_ADD_INCLUDEDIRS
    ./
)

file(GLOB SRCS
    hello_world_main.cpp
    test_wasm3.c
)

set(COMPONENT_SRCS ${SRCS})

set(CXX_COMPILE_OPTIONS
)

register_component()
