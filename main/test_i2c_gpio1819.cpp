#include "Arduino.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "hal/gpio_hal.h"

#include "M5_ENV.h"
#include "Unit_Sonic.h"

#include "test_i2c_gpio1819.h"

static const char *TAG = "test_i2c_gpio1819.cpp";

// I2C Sensor
//  M5STACK ENV III Unit
//  https://docs.m5stack.com/en/unit/envIII
SHT3X sht30;
QMP6988 qmp6988;
bool unitenviii_connected;

// I2C UltraSonic Sensor
//  M5STACK UNIT SONIC I2C
//  https://docs.m5stack.com/en/unit/sonic.i2c
SONIC_I2C rcwl9620;
bool rcwl9620_connected;

#define I2C_SDA 18
#define I2C_SCL 19

void init_i2c_gpio1819(void)
{
    // components/unitenv/UNIT_ENV/src/SHT3X.cpp
    // SHT3X::SHT3X(uint8_t address)
    // {
    //     Wire.begin();
    //     _address=address;
    // }
    //
    // Force i2c reassignment because Wire is initialized in the SHT3X constructor.
    Wire.end();
    // or i2c_set_pin(0, I2C_SDA, I2C_SCL, true, true, I2C_MODE_MASTER);
    Wire.begin(I2C_SDA, I2C_SCL);

    // Unit ENV III
    unitenviii_connected = false;
    if(qmp6988.init()) {
        ESP_LOGI(TAG, "I2C: Found Unit ENV III");
        unitenviii_connected = true;
    } else {
        ESP_LOGI(TAG, "I2C: Not found Unit ENV III");
    }

    // Unit UltraSonic
    rcwl9620_connected = false;
    Wire.beginTransmission(0x57);
    if(Wire.endTransmission() == 0) {
        ESP_LOGI(TAG, "I2C: Found Unit UltraSonic");
        rcwl9620.begin(&Wire);
        rcwl9620_connected = true;
    } else {
        ESP_LOGI(TAG, "I2C: Not found Unit UltraSonic");
    }
}

void get_i2c_unit_data(unitenv_t *unitenv, unit_ultrasonic_t *ultrasonic)
{
    // Unit ENV III
    if(unitenviii_connected) {
        unitenv->pressure = qmp6988.calcPressure() / 100;
        if(sht30.get() == 0) {
            unitenv->tmp = sht30.cTemp;
            unitenv->hum = sht30.humidity;
        } else {
            unitenv->tmp = 0;
            unitenv->hum = 0;
        }
    } else {
        unitenv->pressure = 0;
        unitenv->tmp = 0;
        unitenv->hum = 0;
    }

    // Unit UltraSonic
    if(rcwl9620_connected) {
        ultrasonic->distance = rcwl9620.getDistance();
    } else {
        ultrasonic->distance = 0;
    }
}
