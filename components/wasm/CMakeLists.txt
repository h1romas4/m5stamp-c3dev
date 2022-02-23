cmake_minimum_required(VERSION 3.5)

set(COMPONENT_ADD_INCLUDEDIRS
    wasm3/source
    wasm3/source/extensions
    wasm3/source/extra
)

file(GLOB SRCS
    wasm3/m3_api_libc.c
    wasm3/m3_api_meta_wasi.c
    wasm3/m3_api_tracer.c
    wasm3/m3_api_uvwasi.c
    wasm3/m3_api_wasi.c
    wasm3/m3_bind.c
    wasm3/m3_code.c
    wasm3/m3_compile.c
    wasm3/m3_core.c
    wasm3/m3_env.c
    wasm3/m3_exec.c
    wasm3/m3_function.c
    wasm3/m3_info.c
    wasm3/m3_module.c
    wasm3/m3_parse.c
    wasm3/extension/m3_extensions.c
)

set(COMPONENT_SRCS ${SRCS})

set(C_COMPILE_OPTIONS
    "-DESP32"
    "-DM3_IN_IRAM"
    "-O3"
    "-freorder-blocks"
    # "-Dd_m3LogOutput=true"
    # "-Dd_m3VerboseLogs=true"
    # "-Dd_m3FixedHeap=96000"
    # "-Dd_m3MaxFunctionStackHeight=128"
    # "-Dd_m3CodePageAlignSize=1024"
    # "-Dd_m3EnableOptimizations=1"
)

register_component()
