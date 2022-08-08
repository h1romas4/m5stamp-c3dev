cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    lwgps/lwgps/src/include
)

set(SRCS
    lwgps/lwgps/src/lwgps/lwgps.c
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS})

target_compile_options(${COMPONENT_TARGET} PUBLIC
    -O3
    -DLWGPS_IGNORE_USER_OPTS=1
    -DLWGPS_CFG_STATEMENT_GPGGA=1
    -DLWGPS_CFG_STATEMENT_GPGSA=1
)
