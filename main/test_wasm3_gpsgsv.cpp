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
#include "test_uart_gpio1819.h"

static const char *TAG = "test_wasm3_gpsgsv.cpp";

/**
 * Wasm3 member
 */
IM3Environment wasm3_env;
IM3Runtime wasm3_runtime;
IM3Module wasm3_module;

IM3Function wasm3_func_create_satellites_array;
IM3Function wasm3_func_set_satellites;
IM3Function wasm3_func_set_gsv;
IM3Function wasm3_func_tick;
IM3Function wasm3_func_pin;
IM3Function wasm3_func_unpin;
IM3Function wasm3_func_collect;

uint8_t *wasm_mem;

#define WASM3_STACK_SIZE 16384
#define FREETYPE_FONT_SIZE 9

/**
 * Unit GPS member
 */
#define MAX_SATELLITE 12
unitgpsgsv_t unitgpsgsv[MAX_SATELLITE];
uint32_t wasm_satellites_ptr;
uint32_t wasm_satellites_real;

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

    sprintf(str_32bit, "%ld", wasm_prt);
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

    // ESP_LOGE(TAG, "c3dev_abort: %ls %ls %d %d", message, fileName, lineNumber, columnNumber);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_now)
{
    m3ApiReturnType(int64_t)

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_ms = (int64_t)tv_now.tv_sec * 1000L + /* UTC+9 */ 3600000 * 9;
    // ESP_LOGI(TAG, "c3dev_now: %d", time_ms);

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

M3Result link_c3dev(IM3Runtime runtime) {
    IM3Module module = runtime->modules;

    m3_LinkRawFunction(module, "env", "seed", "F()",  &c3dev_random);
    m3_LinkRawFunction(module, "env", "abort", "v(**ii)",  &c3dev_abort);
    m3_LinkRawFunction(module, "c3dev", "now", "I()",  &c3dev_now);
    m3_LinkRawFunction(module, "c3dev", "delay", "v(i)",  &c3dev_delay);
    m3_LinkRawFunction(module, "c3dev", "draw_pixel", "v(iii)",  &c3dev_draw_pixel);
    m3_LinkRawFunction(module, "c3dev", "draw_line", "v(iiiii)",  &c3dev_draw_line);
    m3_LinkRawFunction(module, "c3dev", "draw_string", "v(iii*)",  &c3dev_draw_string);
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
    result = m3_FindFunction(&wasm3_func_pin, wasm3_runtime, "__pin");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction:wasm3_func_pin: %s", result);
        return ESP_FAIL;
    }
    result = m3_FindFunction(&wasm3_func_collect, wasm3_runtime, "__collect");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction:wasm3_func_collect: %s", result);
        return ESP_FAIL;
    }

    // Draw Clock
    IM3Function gpsgsv;
    result = m3_FindFunction(&gpsgsv, wasm3_runtime, "gpsgsv");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }
    // export function gpsgsv(x: u32, y: u32, r: u32): void;
    const char* i_argv[4] = { "80", "64", "63" };
    result = m3_CallArgv(gpsgsv, 3, i_argv);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    // Get AS function
    result = m3_FindFunction(&wasm3_func_tick, wasm3_runtime, "tick");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }
    result = m3_FindFunction(&wasm3_func_create_satellites_array, wasm3_runtime, "createSatellitesArray");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }
    result = m3_FindFunction(&wasm3_func_set_satellites, wasm3_runtime, "setSatellites");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }
    result = m3_FindFunction(&wasm3_func_set_gsv, wasm3_runtime, "setGsv");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Wasm3 Initialized");
    ESP_LOGI(TAG, "heap_caps_get_free_size: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));

    return ESP_OK;
}

esp_err_t gpsgsv_init_wasm(void)
{
    SPIFFS_WASM.begin(false, "/wasm", 4, "wasm");

    File wasm_file = SPIFFS_WASM.open("/gpsgsv.wasm", "rb");
    size_t wasm_size = wasm_file.size();

    ESP_LOGI(TAG, "gpsgsv.wasm: %d", wasm_size);
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
    if(load_wasm(wasm_binary, wasm_size) != ESP_OK) {
        return ESP_FAIL;
    }

    // initialize GPS member
    memset(unitgpsgsv, 0, sizeof(unitgpsgsv_t) * 12);

    // Create Array memory
    M3Result result = m3Err_none;
    uint32_t p0 = 0;
    result = m3_Call(wasm3_func_create_satellites_array, 0, nullptr);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }
    result = m3_GetResultsV(wasm3_func_create_satellites_array, &p0);
    if (result) {
        ESP_LOGE(TAG, "m3_GetResultsV: %s", result);
        return ESP_FAIL;
    }
    // Pined
    uint32_t *i_argv[2] = { &p0 };
    wasm_satellites_ptr = 0;
    result = m3_Call(wasm3_func_pin, 1, (const void**)i_argv);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }
    result = m3_GetResultsV(wasm3_func_pin, &wasm_satellites_ptr);
    if (result) {
        ESP_LOGE(TAG, "m3_GetResultsV: %s", result);
        return ESP_FAIL;
    }
    // Get memory pointer
    wasm_mem = m3_GetMemory(wasm3_runtime, 0, 0);
    wasm_satellites_real = wasm_mem[wasm_satellites_ptr + 0];
    wasm_satellites_real += wasm_mem[wasm_satellites_ptr + 1] << 8;
    wasm_satellites_real += wasm_mem[wasm_satellites_ptr + 2] << 16;
    wasm_satellites_real += wasm_mem[wasm_satellites_ptr + 3] << 32;

    return ESP_OK;
}

esp_err_t gpsgsv_tick_wasm(bool clear)
{
    // get GPS data
    get_uart_gpsgsv_data(unitgpsgsv, &wasm_mem[wasm_satellites_real]);

    M3Result result = m3Err_none;

    // set GSV
    for(uint32_t i = 0; i < MAX_SATELLITE; i++) {
        uint32_t num = unitgpsgsv[i].num;
        uint32_t elevation = unitgpsgsv[i].elevation;
        uint32_t azimuth = unitgpsgsv[i].azimuth;
        uint32_t snr = unitgpsgsv[i].snr;
        if(num == 0) continue;

        uint32_t *argv0[5] = { &num, &elevation, &azimuth, &snr };
        result = m3_Call(wasm3_func_set_gsv, 4, (const void**)argv0);
        if (result) {
            ESP_LOGE(TAG, "m3_Call: %s", result);
            return ESP_FAIL;
        }
    }

    // set satellites
    uint32_t *argv1[2] = { &wasm_satellites_ptr };
    result = m3_Call(wasm3_func_set_satellites, 1, (const void**)argv1);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    // tick
    uint32_t boolean = 1; // clear ? 1: 0;
    uint32_t *argv2[2] = { &boolean };
    result = m3_Call(wasm3_func_tick, 1, (const void**)argv2);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    // GC by tick for AssemblyScript --runtime minimal
    as_gc_collect();

    return ESP_OK;
}
