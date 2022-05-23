#include "Arduino.h"
#include "esp_log.h"
#include "TinyGPSPlus.h"

#include "test_uart_gpio1819.h"

static const char *TAG = "test_uart_gpio1819.cpp";

// The TinyGPSPlus object
TinyGPSPlus gps;
// Hardware Serial
HardwareSerial GPSRaw(1);

void init_uart_gpio1819(void)
{
    GPSRaw.begin(9600);
    // GPS init
    GPSRaw.setPins(19, 18);
}

void get_i2c_unitgps_data()
{
    while(GPSRaw.available() > 0) {
        // encode GPS
        if(gps.encode(GPSRaw.read())) {
            // output GPS data
            if(gps.location.isValid()) {
                ESP_LOGI(TAG, "Location(lat, lng): (%lf, %lf)", gps.location.lat(), gps.location.lng());
            }
            if(gps.date.isValid()) {
                ESP_LOGI(TAG, "Date: %d/%d/%d", gps.date.year(), gps.date.month(), gps.date.day());
            }
            if(gps.time.isValid()) {
                ESP_LOGI(TAG, "Time: %d:%d:%d.%d", gps.time.hour(), gps.time.minute(), gps.time.second(), gps.time.centisecond());
            }
        }
    }
}
