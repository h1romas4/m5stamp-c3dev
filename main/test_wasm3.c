
#include <stdint.h>
#include "esp_log.h"
#include "esp_err.h"

#include "wasm3.h"
#include "m3_env.h"
#include "fib32.wasm.h"

static const char *TAG = "test_wasm3.c";

esp_err_t load_wasm(void)
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)fib32_wasm;
    uint32_t fsize = fib32_wasm_len;

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
    result = m3_ParseModule(env, &module, wasm, fsize);
    if (result) {
        ESP_LOGE(TAG, "m3_ParseModule: %s", result);
        return ESP_FAIL;
    }

    result = m3_LoadModule(runtime, module);
    if (result) {
        ESP_LOGE(TAG, "m3_LoadModule: %s", result);
        return ESP_FAIL;
    }

    IM3Function f;
    result = m3_FindFunction(&f, runtime, "fib");
    if (result) {
        ESP_LOGE(TAG, "m3_FindFunction: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Running...");

    result = m3_CallV(f, 24);
    if (result) {
        ESP_LOGE(TAG, "m3_Call: %s", result);
        return ESP_FAIL;
    }

    unsigned value = 0;
    result = m3_GetResultsV(f, &value);
    if (result) {
        ESP_LOGE(TAG, "m3_GetResults: %s", result);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Result: %u", value);

    return ESP_OK;
}
