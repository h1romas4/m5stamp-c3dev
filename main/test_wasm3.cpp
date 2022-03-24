#include "Arduino.h"
#include "SPIFFS.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include <stdint.h>

#include "wasm3.h"
#include "m3_env.h"
#include "m3_api_libc.h"

#include "c3dev_board.h"
#include "test_freetype.h"

static const char *TAG = "test_wasm3.cpp";

/**
 * Wasm3 member
 */
IM3Environment wasm3_env;
IM3Runtime wasm3_runtime;
IM3Module wasm3_module;
IM3Function wasm3_func_tick;
IM3Function wasm3_func_unpin;
IM3Function wasm3_func_collect;

#define WASM3_STACK_SIZE 16384
#define FREETYPE_FONT_SIZE 9

/**
 * SPIFFS member
 */
fs::SPIFFSFS SPIFFS_WASM;

/**
 * FreeType member
 */
font_render_t wasm_font_render;

/**
 * as_gc_unpin_ptr
 *
 * ex. as_gc_unpin_ptr(m3ApiPtrToOffset(string_ptr));
 */
void as_gc_unpin_ptr(uint32_t wasm_prt)
{
    M3Result result = m3Err_none;

    char str_32bit[11];

    sprintf(str_32bit, "%d", wasm_prt);
    ESP_LOGI(TAG, "as_gc_unpin_ptr: %s", str_32bit);

    const char* i_argv[1] = { str_32bit };
    result = m3_CallArgv(wasm3_func_unpin, 1, i_argv);
    if (result) {
        ESP_LOGE(TAG, "m3_Call:as_gc_unpin_ptr: %s", result);
    }
}

/**
 * as_gc_collect.
 */
void as_gc_collect(void)
{
    M3Result result = m3Err_none;

    result = m3_Call(wasm3_func_collect, 0, nullptr);
    if (result) {
        ESP_LOGE(TAG, "m3_Call:as_gc_collect: %s", result);
    }
}

// (import "env" "seed" (func $env.seed (type $t3)))
// (type $t3 (func (result f64)))
m3ApiRawFunction(c3dev_random) {
    m3ApiReturnType(float_t)       // 32bit
    m3ApiReturn(esp_random());     // uint32_t */
}

// (import "env" "abort" (func $env.abort (type $t0)))
// (type $t0 (func (param i32 i32 i32 i32)))
m3ApiRawFunction(c3dev_abort)
{
    m3ApiGetArgMem(char16_t *, message)
    m3ApiGetArgMem(char16_t *, fileName)
    m3ApiGetArg(int32_t, lineNumber)
    m3ApiGetArg(int32_t, columnNumber)

    ESP_LOGE(TAG, "c3dev_abort: %ls %ls %d %d", message, fileName, lineNumber, columnNumber);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_now)
{
    m3ApiReturnType(int64_t)

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_ms = (int64_t)tv_now.tv_sec * 1000L + /* UTC+9 */ 3600000 * 9;
    ESP_LOGI(TAG, "c3dev_now: %d", time_ms);

    m3ApiReturn(time_ms);
}

m3ApiRawFunction(c3dev_delay) {
    m3ApiGetArg(int32_t, wait)

    delay(wait);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_draw_pixel)
{
    m3ApiGetArg(int32_t, x)
    m3ApiGetArg(int32_t, y)
    m3ApiGetArg(int32_t, color)

    tft.drawPixel(x, y, color);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_draw_line)
{
    m3ApiGetArg(int32_t, x0)
    m3ApiGetArg(int32_t, y0)
    m3ApiGetArg(int32_t, x1)
    m3ApiGetArg(int32_t, y1)
    m3ApiGetArg(int32_t, color)

    tft.drawLine(x0, y0, x1, y1, color);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_log)
{
    m3ApiGetArgMem(char *, utf8_null_terminated_string)

    ESP_LOGI(TAG, "c3dev_log: %s", utf8_null_terminated_string);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_draw_string)
{
    m3ApiGetArg(int32_t, x)
    m3ApiGetArg(int32_t, y)
    m3ApiGetArg(int32_t, color)
    m3ApiGetArgMem(char *, utf8_null_terminated_string)

    draw_freetype_string(utf8_null_terminated_string, x, y + FREETYPE_FONT_SIZE, color, &wasm_font_render);
    // ESP_LOGI(TAG, "draw_freetype_string(%d, %d, %d, %s)", x, y, color, utf8_null_terminated_string);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_get_env_tmp) {
    m3ApiReturnType(float_t)
    m3ApiReturn(/* dummy */20.0);
}

m3ApiRawFunction(c3dev_get_env_hum) {
    m3ApiReturnType(float_t)
    m3ApiReturn(/* dummy */40.0);
}

m3ApiRawFunction(c3dev_get_env_pressure) {
    m3ApiReturnType(float_t)
    m3ApiReturn(/* dummy */1000.0);
}

M3Result link_c3dev(IM3Runtime runtime) {
    IM3Module module = runtime->modules;

    m3_LinkRawFunction(module, "env", "seed", "F()",  &c3dev_random);
    m3_LinkRawFunction(module, "env", "abort", "v(**ii)",  &c3dev_abort);
    m3_LinkRawFunction(module, "c3dev", "now", "I()",  &c3dev_now);
    m3_LinkRawFunction(module, "c3dev", "delay", "v(i)",  &c3dev_delay);
    m3_LinkRawFunction(module, "c3dev", "draw_pixel", "v(iii)",  &c3dev_draw_pixel);
    m3_LinkRawFunction(module, "c3dev", "draw_line", "v(iiiii)",  &c3dev_draw_line);
    m3_LinkRawFunction(module, "c3dev", "draw_string", "v(iii*)",  &c3dev_draw_string);
    m3_LinkRawFunction(module, "c3dev", "get_env_tmp", "f()",  &c3dev_get_env_tmp);
    m3_LinkRawFunction(module, "c3dev", "get_env_hum", "f()",  &c3dev_get_env_hum);
    m3_LinkRawFunction(module, "c3dev", "get_env_pressure", "f()",  &c3dev_get_env_pressure);
    m3_LinkRawFunction(module, "c3dev", "log", "v(*)",  &c3dev_log);

    return m3Err_none;
}

esp_err_t load_wasm(uint8_t *wasm_binary, size_t wasm_size)
{
    M3Result result = m3Err_none;

    ESP_LOGI(TAG, "heap_caps_get_free_size: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));

    ESP_LOGI(TAG, "Loading WebAssembly...");
    wasm3_env = m3_NewEnvironment();
    if (!wasm3_env) {
        ESP_LOGE(TAG, "m3_NewEnvironment failed");
        return ESP_FAIL;
    }

    wasm3_runtime = m3_NewRuntime(wasm3_env, WASM3_STACK_SIZE, NULL);
    if (!wasm3_runtime) {
        ESP_LOGE(TAG, "m3_NewRuntime failed");
        return ESP_FAIL;
    }

    result = m3_ParseModule(wasm3_env, &wasm3_module, wasm_binary, wasm_size);
    if (result) {
        ESP_LOGE(TAG, "m3_ParseModule: %s", result);
        return ESP_FAIL;
    }

    result = m3_LoadModule(wasm3_runtime, wasm3_module);
    if (result) {
        ESP_LOGE(TAG, "m3_LoadModule: %s", result);
        return ESP_FAIL;
    }

    // link c3dev function
    result = link_c3dev(wasm3_runtime);
    if (result) {
        ESP_LOGE(TAG, "link_c3dev: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Running...");

    // Get AssemblyScript GC interface
    result = m3_FindFunction(&wasm3_func_unpin, wasm3_runtime, "__unpin");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction:wasm3_func_unpin: %s", result);
        return ESP_FAIL;
    }
    result = m3_FindFunction(&wasm3_func_collect, wasm3_runtime, "__collect");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction:wasm3_func_collect: %s", result);
        return ESP_FAIL;
    }

    // ToDo: Workaround: Initialize Wasm3/AssemblyScript GC
    // It appears that the stack will be destroyed if the function is not called once.
    IM3Function init;
    result = m3_FindFunction(&init, wasm3_runtime, "init");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }
    result = m3_Call(init, 0, nullptr);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    // Draw Clock
    IM3Function clock;
    result = m3_FindFunction(&clock, wasm3_runtime, "clock");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }
    // export function clock(x: u32, y: u32, r: u32): void;
    const char* i_argv[4] = { "80", "64", "63" };
    result = m3_CallArgv(clock, 3, i_argv);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    // Get tick function
    result = m3_FindFunction(&wasm3_func_tick, wasm3_runtime, "tick");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Wasm3 Initialized");
    ESP_LOGI(TAG, "heap_caps_get_free_size: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));

    return ESP_OK;
}

esp_err_t init_wasm(void)
{
    SPIFFS_WASM.begin(false, "/wasm", 4, "wasm");

    File wasm_file = SPIFFS_WASM.open("/app.wasm", "rb");
    size_t wasm_size = wasm_file.size();

    ESP_LOGI(TAG, "app.wasm: %d", wasm_size);
    // Read .wasm
    uint8_t *wasm_binary = (uint8_t *)malloc(sizeof(uint8_t) * wasm_size);
    if(wasm_binary == nullptr) {
        ESP_LOGE(TAG, "Memory alloc error");
        return ESP_FAIL;
    }
    if(wasm_file.read(wasm_binary, wasm_size) != wasm_size) {
        ESP_LOGE(TAG, "SPIFFS read error");
        return ESP_FAIL;
    }

    wasm_file.close();
    SPIFFS_WASM.end();

    // Setup Font render
    wasm_font_render = create_freetype_render(FREETYPE_FONT_SIZE, /* font cache */ 64);
    // Clear LCD
    tft.fillScreen(ST77XX_BLACK);

    // Load WebAssembly on Wasm3
    return load_wasm(wasm_binary, wasm_size);
}

esp_err_t tick_wasm(void)
{
    M3Result result = m3Err_none;

    result = m3_Call(wasm3_func_tick, 0, nullptr);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    // GC by tick for AssemblyScript --runtime minimal
    as_gc_collect();

    return ESP_OK;
}
