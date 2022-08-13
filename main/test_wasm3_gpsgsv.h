#include <stdint.h>

void as_gc_unpin_ptr(uint32_t wasm_prt);
void as_gc_collect(void);
esp_err_t gpsgsv_init_wasm(void);
esp_err_t gpsgsv_tick_wasm(void);
