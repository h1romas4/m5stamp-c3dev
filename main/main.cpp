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

#ifdef CONFIG_GPIO1819_UNITENV_III
#include "test_i2c_gpio1819.h"
#endif
#ifdef CONFIG_GPIO1819_UNIT_GPS
#include "test_uart_gpio1819.h"
#else
#include "test_wasm3_clockenv.h"
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

    // RGB LED initialize
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(16, 0, 16));
    pixels.show();

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
        // but RTC is cleared at reboot in esp-idf 4.4.1 and later.
        // Time synchronization is currently not possible. (TODO)
        esp_restart();
    }

    #ifdef CONFIG_GPIO1819_UNITENV_III
    init_i2c_gpio1819();
    #endif
    #ifdef CONFIG_GPIO1819_UNIT_GPS
    init_uart_gpio1819();
    #endif

    // Test WebAssembly
    if(init_wasm() == ESP_OK) enable_wasm = true;
}

void loop(void)
{
    // Test Switch
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    // Test WebAssembly
    if(enable_wasm) tick_wasm();
    delay(500);
}
