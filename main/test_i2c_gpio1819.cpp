#include "Arduino.h"
#include "esp_log.h"
#include "hal/gpio_hal.h"
#include "UNIT_ENV.h"

static const char *TAG = "test_i2c_gpio1819.cpp";

SHT3X sht30;
QMP6988 qmp6988;

#define I2C_SDA 18
#define I2C_SCL 19

void init_i2c_gpio1819(void)
{
    // Disable JTAG (not working yet)
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[I2C_SDA], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[I2C_SCL], PIN_FUNC_GPIO);
    pinMode(I2C_SDA, INPUT | OUTPUT);
    pinMode(I2C_SCL, INPUT | OUTPUT);
    digitalWrite(I2C_SDA, LOW);
    digitalWrite(I2C_SCL, LOW);

    Wire.begin(I2C_SDA, I2C_SCL);
    if(!qmp6988.init()) {
        ESP_LOGE(TAG, "Could not find a valid qmp6988 sensor");
    }
}
