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
    m3ApiReturnType (int32_t)
    m3ApiGetArg     (int32_t, max)
    m3ApiReturn     (esp_random(/* max */));
}

m3ApiRawFunction(c3dev_delay) {
    m3ApiGetArg     (int32_t, wait)
    delay(wait);
    m3ApiSuccess();
}

m3ApiRawFunction(c3dev_pset)
{
    m3ApiGetArg     (int32_t, x)
    m3ApiGetArg     (int32_t, y)
    m3ApiGetArg     (int32_t, color)

    tft.drawPixel(x, y, color);

    m3ApiSuccess();
}

M3Result link_c3dev(IM3Runtime runtime) {
    IM3Module module = runtime->modules;

    m3_LinkRawFunction(module, "c3dev", "random", "i(i)",  &c3dev_random);
    m3_LinkRawFunction(module, "c3dev", "delay", "v(i)",  &c3dev_delay);
    m3_LinkRawFunction(module, "c3dev", "pset", "v(iii)",  &c3dev_pset);

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

    return ESP_OK;
}
