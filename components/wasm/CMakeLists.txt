cmake_minimum_required(VERSION 3.5)

set(INCLUDEDIRS
    wasm3/source
    wasm3/source/extensions
    wasm3/source/extra
)

set(SRCS
    wasm3/source/m3_api_libc.c
    wasm3/source/m3_api_wasi.c
    wasm3/source/m3_api_uvwasi.c
    wasm3/source/m3_api_meta_wasi.c
    wasm3/source/m3_api_tracer.c
    wasm3/source/m3_bind.c
    wasm3/source/m3_code.c
    wasm3/source/m3_compile.c
    wasm3/source/m3_core.c
    wasm3/source/m3_env.c
    wasm3/source/m3_exec.c
    wasm3/source/m3_function.c
    wasm3/source/m3_info.c
    wasm3/source/m3_module.c
    wasm3/source/m3_parse.c
)

idf_component_register(INCLUDE_DIRS ${INCLUDEDIRS} SRCS ${SRCS})

target_compile_options(${COMPONENT_TARGET} PUBLIC
    -DESP32
    -DM3_IN_IRAM
    -O3
    # -Dd_m3FixedHeap=131070
    -DDEBUG
    -Dd_m3LogNativeStack
    -Dd_m3LogHeapOps
    -Dd_m3SkipStackCheck
    -Dd_m3SkipMemoryBoundsCheck
    -Wno-visibility)

set_source_files_properties(wasm3/source/m3_env.c
    PROPERTIES COMPILE_FLAGS
    -Wno-sign-compare
)

set_source_files_properties(wasm3/source/m3_api_meta_wasi.c
    PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types
)
