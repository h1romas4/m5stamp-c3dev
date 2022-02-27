import * as c3dev from "./c3dev";

export function circle_fill_random(x: u32, y: u32): void {
    for (let r: u32 = 0; r < x / 2; r++) {
        circle(x / 2, y / 2, r, (u16)(Math.random() * 65535));
    }
}

export function circle(x: u32, y: u32, r: u32, color: u16): void {
    let xx: u32 = r;
    let yy: u32 = 0;
    let err = 0;

    // c3dev.drawString(String.UTF8.encode("TEST STRING1", true));
    // c3dev.drawString(String.UTF8.encode("TEST STRING2", true));
    // c3dev.delay(100);
    // c3dev.drawString(String.UTF8.encode("TEST STRING3", true));

    // c3dev.drawString(new ArrayBuffer(10));
    // String.UTF8.encode("TEST STRING1");

    new ArrayBuffer(1);

    // Stack dump detected
    // Core  0 register dump:
    // MEPC    : 0x40387ae0  RA      : 0x40387b50  SP      : 0x3fcc23b0  GP      : 0x3fc8aa00
    // 0x40387ae0: search_suitable_block at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:183
    //  (inlined by) block_locate_free at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:441
    //  (inlined by) tlsf_malloc at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:757
    //
    // 0x40387b50: tlsf_fls at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:69
    //  (inlined by) mapping_insert at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:155
    //  (inlined by) mapping_search at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:171
    //  (inlined by) block_locate_free at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:431
    //  (inlined by) tlsf_malloc at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:757
    //
    // TP      : 0x3fcacddc  T0      : 0x4005890e  T1      : 0x00000000  T2      : 0x00000000
    // S0/FP   : 0x00000400  S1      : 0x0000041f  A0      : 0x00000015  A1      : 0x00000400
    // A2      : 0x00000001  A3      : 0x00000004  A4      : 0x00000000  A5      : 0x00000020
    // A6      : 0x00000000  A7      : 0x00000000  S2      : 0x00000000  S3      : 0x00000004
    // S4      : 0x00000000  S5      : 0x00000000  S6      : 0x00000000  S7      : 0x00000000
    // S8      : 0x00000000  S9      : 0x00000000  S10     : 0x00000000  S11     : 0x00000000
    // T3      : 0x00000020  T4      : 0x00000000  T5      : 0xffffffff  T6      : 0xffffffff
    // MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000005  MTVAL   : 0x00000024
    // 0x40380001: _vector_table at ??:?
    //
    // MHARTID : 0x00000000
    //
    // Backtrace:
    //
    // 0x40387ae0 in search_suitable_block (sli=<synthetic pointer>, fli=<synthetic pointer>, control=0x0) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:441
    // 441                             block = search_suitable_block(control, &fl, &sl);
    // #0  0x40387ae0 in search_suitable_block (sli=<synthetic pointer>, fli=<synthetic pointer>, control=0x0) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:441
    // #1  block_locate_free (size=1024, control=0x0) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:441
    // #2  tlsf_malloc (tlsf=0x0, size=size@entry=1024) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_tlsf.c:757
    // #3  0x403887ee in multi_heap_malloc_impl (heap=0x3fcc0000, size=size@entry=1024) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/multi_heap.c:197
    // #4  0x403808fc in heap_caps_malloc (size=1024, caps=caps@entry=6144) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_caps.c:145
    // #5  0x4038097a in heap_caps_malloc_default (size=size@entry=1024) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/heap/heap_caps.c:177
    // #6  0x40388d28 in _calloc_r (r=<optimized out>, nmemb=nmemb@entry=1024, size=size@entry=1) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/newlib/heap.c:72
    // #7  0x40388d62 in calloc (n=1024, size=1) at /home/hiromasa/devel/toolchain/esp/esp-idf/components/newlib/heap.c:36
    // #8  0x42040a20 in m3_Malloc_Impl (i_size=1024) at ../components/wasm/wasm3/source/m3_core.c:129
    // #9  0x42044756 in NewCodePage (i_runtime=0x3fcda53c, i_minNumLines=8) at ../components/wasm/wasm3/source/m3_code.c:21
    // #10 0x4204274a in AcquireCodePageWithCapacity (i_runtime=0x3fcda53c, i_minLineCount=8) at ../components/wasm/wasm3/source/m3_env.c:1058
    // #11 0x4203b678 in EnsureCodePageNumLines (o=0x3fcda53c, i_numLines=8) at ../components/wasm/wasm3/source/m3_compile.c:34
    // #12 0x4203b710 in EmitOp (o=0x3fcda53c, i_operation=0x42034c80 <op_If_r>) at ../components/wasm/wasm3/source/m3_compile.c:68
    // #13 0x4203f354 in Compile_If (o=0x3fcda53c, i_opcode=4) at ../components/wasm/wasm3/source/m3_compile.c:1942
    // #14 0x4203fe8e in CompileBlockStatements (o=0x3fcda53c) at ../components/wasm/wasm3/source/m3_compile.c:2608
    // #15 0x42040278 in CompileBlock (o=0x3fcda53c, i_blockType=0x3fcd9cec, i_blockOpcode=3) at ../components/wasm/wasm3/source/m3_compile.c:2739
    // #16 0x4203f1c6 in Compile_LoopOrBlock (o=0x3fcda53c, i_opcode=3) at ../components/wasm/wasm3/source/m3_compile.c:1895
    // #17 0x4203fe8e in CompileBlockStatements (o=0x3fcda53c) at ../components/wasm/wasm3/source/m3_compile.c:2608
    // #18 0x42040278 in CompileBlock (o=0x3fcda53c, i_blockType=0x3fcd9cec, i_blockOpcode=2) at ../components/wasm/wasm3/source/m3_compile.c:2739
    // #19 0x4203f1c6 in Compile_LoopOrBlock (o=0x3fcda53c, i_opcode=2) at ../components/wasm/wasm3/source/m3_compile.c:1895
    // #20 0x4203fe8e in CompileBlockStatements (o=0x3fcda53c) at ../components/wasm/wasm3/source/m3_compile.c:2608
    // #21 0x42040912 in CompileFunction (io_function=0x3fcdc0ac) at ../components/wasm/wasm3/source/m3_compile.c:2899
    // #22 0x420349ec in op_Compile (_pc=0x3fcdc240, _sp=0x3fcdba98, _mem=0x3fc8f8b4, _r0=0, _fp0=0) at ../components/wasm/wasm3/source/m3_exec.h:760
    // #23 0x00000000 in ?? ()    //

    // TODO: All the variables under this will be broken.

    while(xx >= yy) {
        c3dev.pset(x + xx, y + yy, color);
        c3dev.pset(x + yy, y + xx, color);
        c3dev.pset(x - yy, y + xx, color);
        c3dev.pset(x - xx, y + yy, color);
        c3dev.pset(x - xx, y - yy, color);
        c3dev.pset(x - yy, y - xx, color);
        c3dev.pset(x + yy, y - xx, color);
        c3dev.pset(x + xx, y - yy, color);
        if(err <= 0) {
            yy++;
            err += 2 * yy + 1;
        } else {
            xx--;
            err -= 2 * xx + 1;
        }
    }
}
