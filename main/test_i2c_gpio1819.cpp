#include "Arduino.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "hal/gpio_hal.h"
#include "UNIT_ENV.h"

#include "test_i2c_gpio1819.h"

static const char *TAG = "test_i2c_gpio1819.cpp";

SHT3X sht30;
QMP6988 qmp6988;

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
    i2c_set_pin(0, I2C_SDA, I2C_SCL, true, true, I2C_MODE_MASTER);

    Wire.begin(I2C_SDA, I2C_SCL);
    if(!qmp6988.init()) {
        ESP_LOGE(TAG, "Could not find a valid qmp6988 sensor");
    }
}

void get_i2c_unitenv_data(unitenv_t *unitenv)
{
    unitenv->pressure = qmp6988.calcPressure() / 100;
    if(sht30.get() == 0) {
        unitenv->tmp = sht30.cTemp;
        unitenv->hum = sht30.humidity;
    } else {
        unitenv->tmp = 0;
        unitenv->hum = 0;
    }
}
