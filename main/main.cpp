#include "Arduino.h"
#include "esp_log.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "Adafruit_NeoPixel.h"

#include "c3dev_board.h"
#include "test_freetype.h"
#include "test_tinypng.h"
#include "test_nvs_wifi.h"

#ifdef CONFIG_GPIO1819_NONE
#include "test_wasm3_clockenv.h"
#endif
#ifdef CONFIG_GPIO1819_I2C
#include "test_i2c_gpio1819.h"
#include "test_wasm3_clockenv.h"
#endif
#ifdef CONFIG_GPIO1819_DIGIT_UNITIR
#include "test_digit_gpio1819.h"
#include "test_wasm3_clockenv.h"
#endif
#ifdef CONFIG_GPIO1819_UART_UNITGPS
#include "test_uart_gpio1819.h"
#include "test_wasm3_gpsgsv.h"
#endif
#ifdef CONFIG_GPIO1819_IMU6886
#include "test_wasm3_imu6886.h"
#endif

static const char *TAG = "main.cpp";

/**
 * SPI member
 */
SPIClass *spi = &SPI;

/**
 * LCD member
 */
Adafruit_ST7735 tft = Adafruit_ST7735(spi, C3DEV_LCD_CS, C3DEV_LCD_DC, C3DEV_LCD_RST);

/**
 * RGB LED member
 */
Adafruit_NeoPixel pixels(1, M5STAMP_C3_LED, NEO_GRB + NEO_KHZ800);

/**
 * FreeType member
 */
font_render_t font_render;

/**
 * Wasm3 member
 */
boolean enable_wasm = false;

void setup(void)
{
    // SW initialize
    pinMode(C3DEV_SW1, INPUT); // external pull up
    pinMode(M5STAMP_C3_SW, INPUT_PULLUP);

    // SPI initialize
    SPI.begin(C3DEV_SPI_SCLK, C3DEV_SPI_MISO, C3DEV_SPI_MOSI, C3DEV_SD_CS);
    SPI.setFrequency(C3DEV_SPI_CLOCK);

    // LCD initialize
    tft.initR(INITR_BLACKTAB);
    tft.setSPISpeed(C3DEV_SPI_CLOCK);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);
    // If the color is inverted, set to 1.
    tft.invertDisplay(0);
    // tft.invertDisplay(1);

    // Test FreeType
    init_freetype();
    font_render = create_freetype_render(/* font size */ 20, /* font cache */ 64);
    draw_freetype_string("M5Stamp C3", 10, 28, ST77XX_RED, &font_render);
    draw_freetype_string("Development", 10, 28 * 2, ST77XX_WHITE, &font_render);
    draw_freetype_string("Board", 10, 28 * 3, ST77XX_WHITE, &font_render);
    draw_freetype_string("RISC-V", 10, 28 * 4, ST77XX_BLUE, &font_render);
    delay(1000);

    // Test SD card and PNG
    // draw_sdcard_png("/M5STACK/TEST10-0.PNG", 0, 0);
    // draw_sdcard_png("/M5STACK/TEST10-1.PNG", 80, 0);
    // draw_sdcard_png("/M5STACK/TEST10-2.PNG", 0, 60);
    // draw_sdcard_png("/M5STACK/TEST10-3.PNG", 80, 60);

    // Test NVS and Wifi (Push SW1)
    if(digitalRead(C3DEV_SW1) == 0) {
        sync_wifi_ntp();
        // not enough memory..
        ESP_LOGI(TAG, "Restart ESP32C3");
        esp_restart();
    }

    #ifdef CONFIG_GPIO1819_I2C
    init_i2c_gpio1819();
    #endif
    #ifdef CONFIG_GPIO1819_UART_UNITGPS
    init_uart_gpio1819();
    #endif
    #ifdef CONFIG_GPIO1819_DIGIT_UNITIR
    init_digit_gpio1819();
    #endif

    // Test WebAssembly
    #ifdef CONFIG_GPIO1819_UART_UNITGPS
    if(gpsgsv_init_wasm() == ESP_OK) enable_wasm = true;
    #elif CONFIG_GPIO1819_IMU6886
    if(imu6886_init_wasm() == ESP_OK) enable_wasm = true;
    #else
    if(clockenv_init_wasm() == ESP_OK) enable_wasm = true;
    #endif
}

void loop(void)
{
    // Test Switch
    #ifndef CONFIG_GPIO1819_IMU6886
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    #endif

    // Test GPIO0 ADC (UNIT Light)
    #ifndef CONFIG_GPIO1819_IMU6886
    float_t an = (float_t)analogRead(C3DEV_GPIO_0) / (float_t)4096;
    if(an > 1) an = 1;
    ESP_LOGI(TAG, "GPIO0 analog: %f", an);
    #endif

    // Test RGB LED
    #ifndef CONFIG_GPIO1819_IMU6886
    an = pow(an, 8);
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(255 * an, 8, 255 * an));
    pixels.show();
    #endif

    // Test UNIT IR
    #ifdef CONFIG_GPIO1819_DIGIT_UNITIR
    get_digit_unitir_data();
    #endif

    // Test WebAssembly
    if(enable_wasm) {
        #ifdef CONFIG_GPIO1819_UART_UNITGPS
        // GPS GSV View
        gpsgsv_tick_wasm(digitalRead(C3DEV_SW1) == 0 ? true: false);
        #elif CONFIG_GPIO1819_IMU6886
        // 3D Cube
        // uint32_t time = millis();
        imu6886_tick_wasm();
        // ESP_LOGI(TAG, "time: %d", (uint32_t)(millis() - time));
        // When draw to LCD is suppressed. (calculations only)
        //     I (4057) main.cpp: time: 16
        #else
        // Clock Env
        clockenv_tick_wasm();
        #endif
    }

    #ifdef CONFIG_GPIO1819_IMU6886
    delay(1);
    #else
    delay(500);
    #endif
}
