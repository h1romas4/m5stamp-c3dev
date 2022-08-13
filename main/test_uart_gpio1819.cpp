#include "Arduino.h"
#include "esp_log.h"
#include "lwgps/lwgps.h"

#include "test_uart_gpio1819.h"

static const char *TAG = "test_uart_gpio1819.cpp";

// I2C Sensor
//  M5STACK UNIT GPS
//  https://docs.m5stack.com/en/unit/gps
HardwareSerial GPSRaw(1);

/* GPS handle */
lwgps_t hgps;

void init_uart_gpio1819(void)
{
    GPSRaw.begin(9600);
    // Set pin (connected reverse)
    GPSRaw.setPins(/* GPS TXD (yellow) */ 19, /* GPS RXD (write) */ 18);
    // Init GPS
    lwgps_init(&hgps);

    ESP_LOGI(TAG, "Hardware serial and lwgps initialized.");
}

void get_uart_gpsgsv_data(unitgpsgsv_t unitgpsgsv[], uint8_t *satellites)
{
    uint8_t buffer[255];

    while(GPSRaw.available() > 0) {
        size_t size = GPSRaw.read(buffer, sizeof(buffer));
        if(size > 0) {
            // ESP_LOGI(TAG, "GPS: %s", buffer);
            lwgps_process(&hgps, buffer, size);
        }
        if(hgps.is_valid == 1) {
            ESP_LOGI(TAG, "Latitude: %f degrees", hgps.latitude);
            ESP_LOGI(TAG, "Longitude: %f degrees", hgps.longitude);
            ESP_LOGI(TAG, "Altitude: %f meters", hgps.altitude);
            for(uint8_t i = 0; i < 12; i++) {
                satellites[i] = hgps.satellites_ids[i];
                if(satellites[i] != 0) {
                    ESP_LOGI(TAG, "GSA satellites_ids[%d]=%d", i, satellites[i]);
                }
            }
        }
        for(uint8_t i = 0; i < 12; i++) {
            uint8_t num = hgps.sats_in_view_desc[i].num;
            if(num != 0) {
                unitgpsgsv[i].num = num;
                unitgpsgsv[i].elevation = hgps.sats_in_view_desc[i].elevation;
                unitgpsgsv[i].azimuth = hgps.sats_in_view_desc[i].azimuth;
                unitgpsgsv[i].snr = hgps.sats_in_view_desc[i].snr;
                ESP_LOGI(TAG, "GSV %d, elevation %d, azimuth %d, snr %d",
                    num,
                    unitgpsgsv[i].elevation,
                    unitgpsgsv[i].azimuth,
                    unitgpsgsv[i].snr
                );
            }
        }
    }
}
