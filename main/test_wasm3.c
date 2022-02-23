#include "Arduino.h"
#include "esp_log.h"
#include "esp_err.h"
#include <stdint.h>

#include "wasm3.h"
#include "m3_env.h"
#include "m3_api_libc.h"

#include "c3dev_board.h"

static const char *TAG = "test_wasm3.c";

m3ApiRawFunction(c3dev_random) {
    m3ApiReturnType (uint32_t)
    m3ApiGetArg     (uint32_t, max)
    m3ApiReturn     (esp_random());
}

m3ApiRawFunction(c3dev_pset)
{
    m3ApiGetArg     (uint32_t, x)
    m3ApiGetArg     (uint32_t, y)
    m3ApiGetArg     (uint32_t, color)

    // current_gas -= gas;

    m3ApiSuccess();
}

M3Result link_c3dev(IM3Runtime runtime) {
    IM3Module module = runtime->modules;
    // (type $t0 (func (param i32) (result i32)))
    // (type $t1 (func (param i32 i32 i32)))
    // (import "c3dev" "random" (func $c3dev.random (type $t0)))
    // (import "c3dev" "pset" (func $c3dev.pset (type $t1)))
    m3_LinkRawFunction(module, "c3dev", "random", "i(i)",  &c3dev_random);
    m3_LinkRawFunction(module, "c3dev", "pset", "v(i, i, i)",  &c3dev_pset);

    return m3Err_none;
}

esp_err_t load_wasm(uint8_t *wasm_binary, size_t wasm_size)
{
    M3Result result = m3Err_none;

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

    // link arduino library
    result = link_c3dev(runtime);
    if (result) {
        ESP_LOGE(TAG, "link_c3dev: %s", result);
        return ESP_FAIL;
    }

    // export function circle(x: u32, y: u32, r: u32, color: u16): void
    IM3Function circle;
    result = m3_FindFunction(&circle, runtime, "circle");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Running...");

    result = m3_CallV(circle, 80, 64, 64, 0xffff);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Executed.");

    return ESP_OK;
}
