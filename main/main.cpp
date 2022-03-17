#include "Arduino.h"
#include "WiFi.h"
#include "Preferences.h"
#include "esp_log.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"

#include "c3dev_board.h"
#include "test_freetype.h"
#include "test_tinypng.h"
#include "test_wasm3.h"

static const char *TAG = "main.cpp";

/**
 * SPI member
 */
SPIClass *spi = &SPI;


/**
 * NVS member
 */
Preferences preferences;

/**
 * LCD member
 */
Adafruit_ST7735 tft = Adafruit_ST7735(spi, C3DEV_LCD_CS, C3DEV_LCD_DC, C3DEV_LCD_RST);

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

    // Test NVS and Wifi
    if(preferences.begin("wifi", true)) {
        String ssid = preferences.getString("ssid");
        String passwd = preferences.getString("passwd");
        String ntp1 = preferences.getString("ntp1");
        String ntp2 = preferences.getString("ntp2");
        String ntp3 = preferences.getString("ntp3");

        ESP_LOGI(TAG, "connect to %s", ssid);
        WiFi.begin(ssid.c_str(), passwd.c_str());
        configTime(9 * 3600L, 0, ntp1.c_str(), ntp2.c_str(), ntp3.c_str());
        ESP_LOGI(TAG, "configured time from NTP");
    }


    // Test WebAssembly
    if(init_wasm() == ESP_OK) enable_wasm = true;
}

void loop(void)
{
    // Test Switch
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    // Test WebAssembly
    if(enable_wasm) tick_wasm();
    delay(1000);
}
