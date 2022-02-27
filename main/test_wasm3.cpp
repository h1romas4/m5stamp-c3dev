#include "Arduino.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include <stdint.h>

#include "wasm3.h"
#include "m3_env.h"
#include "m3_api_libc.h"

#include "c3dev_board.h"

static const char *TAG = "test_wasm3.c";

// (import "env" "seed" (func $env.seed (type $t3)))
m3ApiRawFunction(c3dev_random) {
    m3ApiReturnType(float_t)       // 32bit
    m3ApiReturn(esp_random());     // uint32_t */
}

// (import "env" "abort" (func $env.abort (type $t1)))
m3ApiRawFunction(c3dev_abort)
{
    m3ApiGetArgMem(const char *, message)
    m3ApiGetArgMem(const char *, fileName)
    m3ApiGetArg(int32_t, lineNumber)
    m3ApiGetArg(int32_t, columnNumber)

    ESP_LOGE(TAG, "c3dev_abort: %s %s %d %d", message, fileName, lineNumber, columnNumber);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_delay) {
    m3ApiGetArg(int32_t, wait)

    delay(wait);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_pset)
{
    m3ApiGetArg(int32_t, x)
    m3ApiGetArg(int32_t, y)
    m3ApiGetArg(int32_t, color)

    ESP_LOGI(TAG, "pset(%d, %d, %d)", x, y, color);
    tft.drawPixel(x, y, color);

    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_draw_string)
{
    m3ApiGetArgMem(const char *, utf8_null_terminated_string)

    ESP_LOGI(TAG, "%s", utf8_null_terminated_string);

    m3ApiSuccess();
}

M3Result link_c3dev(IM3Runtime runtime) {
    IM3Module module = runtime->modules;

    m3_LinkRawFunction(module, "env", "seed", "F()",  &c3dev_random);
    m3_LinkRawFunction(module, "env", "abort", "v(**ii)",  &c3dev_abort);
    m3_LinkRawFunction(module, "c3dev", "delay", "v(i)",  &c3dev_delay);
    m3_LinkRawFunction(module, "c3dev", "pset", "v(iii)",  &c3dev_pset);
    m3_LinkRawFunction(module, "c3dev", "drawString", "v(*)",  &c3dev_draw_string);

    return m3Err_none;
}

esp_err_t load_wasm(uint8_t *wasm_binary, size_t wasm_size)
{
    M3Result result = m3Err_none;

    ESP_LOGI(TAG, "heap_caps_get_free_size: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));

    ESP_LOGI(TAG, "Loading WebAssembly...");
    IM3Environment env = m3_NewEnvironment();
    if (!env) {
        ESP_LOGE(TAG, "m3_NewEnvironment failed");
        return ESP_FAIL;
    }

    IM3Runtime runtime = m3_NewRuntime(env, 1024, NULL);
    if (!runtime) {
        ESP_LOGE(TAG, "m3_NewRuntime failed");
        return ESP_FAIL;
    }

    IM3Module module;
    result = m3_ParseModule(env, &module, wasm_binary, wasm_size);
    if (result) {
        ESP_LOGE(TAG, "m3_ParseModule: %s", result);
        return ESP_FAIL;
    }

    result = m3_LoadModule(runtime, module);
    if (result) {
        ESP_LOGE(TAG, "m3_LoadModule: %s", result);
        return ESP_FAIL;
    }

    // link c3dev function
    result = link_c3dev(runtime);
    if (result) {
        ESP_LOGE(TAG, "link_c3dev: %s", result);
        return ESP_FAIL;
    }

    IM3Function circle;
    result = m3_FindFunction(&circle, runtime, "circle");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Running...");
    const char* i_argv[4] = { "80", "64", "64", "65535" };
    result = m3_CallArgv(circle, 4, i_argv);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Executed.");

    // wasm3/source/m3_function.c:86
    // Guru Meditation Error: Core  0 panic'ed (Load access fault)
    // m3_FreeModule(module);
    // m3_FreeRuntime(runtime);

    ESP_LOGI(TAG, "heap_caps_get_free_size: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));

    return ESP_OK;
}
