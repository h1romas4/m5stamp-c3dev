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
    CLEAR_PERI_REG_MASK(USB_DEVICE_CONF0_REG, USB_DEVICE_USB_PAD_ENABLE);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[I2C_SDA], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[I2C_SCL], PIN_FUNC_GPIO);
    pinMode(I2C_SDA, INPUT_PULLUP | OUTPUT);
    pinMode(I2C_SCL, INPUT_PULLUP | OUTPUT);
    digitalWrite(I2C_SDA, LOW);
    digitalWrite(I2C_SCL, LOW);

    // https://community.m5stack.com/topic/3715/m5stamp-c3-i2c-interface
    // But I'm still unable to read from the I2C device. I've got other stuff working on the board
    // (like a fan-control, and a water pump) but I2C and Servos are steadfastly refusing to function.
    Wire.begin(I2C_SDA, I2C_SCL);
    if(!qmp6988.init()) {
        ESP_LOGE(TAG, "Could not find a valid qmp6988 sensor");
    }
}
