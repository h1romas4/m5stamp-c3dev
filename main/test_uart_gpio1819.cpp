#include "Arduino.h"
#include "esp_log.h"
#include "lwgps/lwgps.h"

#include "test_uart_gpio1819.h"

static const char *TAG = "test_uart_gpio1819.cpp";

// I2C Sensor
//  M5STACK UNIT GPS
//  https://docs.m5stack.com/en/unit/gps
HardwareSerial GPSRaw(1);

void init_uart_gpio1819(void)
{
    GPSRaw.begin(9600);
    // Set pin (connected reverse)
    GPSRaw.setPins(/* GPS TXD (yellow) */ 19, /* GPS RXD (write) */ 18);

    ESP_LOGI(TAG, "Hardware serial initialized.");
}

void get_i2c_unitgps_data()
{
    while(GPSRaw.available() > 0) {
        // // encode GPS
        // if(gps.encode(GPSRaw.read())) {
        //     // output GPS data
        //     if(gps.location.isValid()) {
        //         ESP_LOGI(TAG, "Location(lat, lng): (%lf, %lf)", gps.location.lat(), gps.location.lng());
        //     }
        //     if(gps.date.isValid()) {
        //         ESP_LOGI(TAG, "Date: %d/%d/%d", gps.date.year(), gps.date.month(), gps.date.day());
        //     }
        //     if(gps.time.isValid()) {
        //         ESP_LOGI(TAG, "Time: %d:%d:%d.%d", gps.time.hour(), gps.time.minute(), gps.time.second(), gps.time.centisecond());
        //     }
        // }
    }
}
