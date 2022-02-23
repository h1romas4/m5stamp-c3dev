set(COMPONENT_ADD_INCLUDEDIRS
    ./
)

file(GLOB SRCS
    hello_world_main.cpp
)

set(COMPONENT_SRCS ${SRCS})

set(CXX_COMPILE_OPTIONS
)

register_component()
