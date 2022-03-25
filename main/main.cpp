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
#include "test_i2c_gpio1819.h"
#include "test_wasm3.h"

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
 * FreeType member
 */
font_render_t font_render_20px;
font_render_t font_render_16px;

/**
 * Wasm3 member
 */
boolean enable_wasm = false;

void sync_wifi_ntp(void)
{
    Preferences preferences;

    if(!preferences.begin("wifi", true)) return;

    String ssid = preferences.getString("ssid");
    String passwd = preferences.getString("passwd");

    ESP_LOGI(TAG, "Connect to %s", ssid);
    WiFi.begin(ssid.c_str(), passwd.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
    }
    ESP_LOGI(TAG, "Connected!");
    configTime(9 * 3600L, 0, "ntp1.jst.mfeed.ad.jp", "ntp2.jst.mfeed.ad.jp", "ntp3.jst.mfeed.ad.jp");
    // Wait Time Sync
    struct tm timeInfo;
    while(true) {
        getLocalTime(&timeInfo);
        if(timeInfo.tm_year > 0) {
            break;
        }
        delay(500);
        ESP_LOGI(TAG, "waiting time sync..(%d)", timeInfo.tm_year);
    }
    ESP_LOGI(TAG, "Configured time from NTP");
    WiFi.disconnect();
    // not enough memory..
    ESP_LOGI(TAG, "Restart ESP32C3");
    esp_restart();
}

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
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    // If the color is inverted, set to 1.
    tft.invertDisplay(0);
    // tft.invertDisplay(1);

    // Test FreeType
    init_freetype();
    font_render_20px = create_freetype_render(/* font size */ 20, /* font cache */ 64);
    draw_freetype_string("M5Stamp C3", 10, 28, ST77XX_RED, &font_render_20px);
    draw_freetype_string("Development", 10, 28 * 2, ST77XX_WHITE, &font_render_20px);
    draw_freetype_string("Board", 10, 28 * 3, ST77XX_WHITE, &font_render_20px);
    draw_freetype_string("RISC-V", 10, 28 * 4, ST77XX_BLUE, &font_render_20px);
    delay(1000);

    // Test SD card and PNG
    // draw_sdcard_png("/M5STACK/TEST10-0.PNG", 0, 0);
    // draw_sdcard_png("/M5STACK/TEST10-1.PNG", 80, 0);
    // draw_sdcard_png("/M5STACK/TEST10-2.PNG", 0, 60);
    // draw_sdcard_png("/M5STACK/TEST10-3.PNG", 80, 60);

    // Test NVS and Wifi (Push SW1)
    if(digitalRead(C3DEV_SW1) == 0) {
        sync_wifi_ntp();
    }

    // Test I2C UnitENV III
    init_i2c_gpio1819();

    // Test WebAssembly
    if(init_wasm() == ESP_OK) enable_wasm = true;
}

void loop(void)
{
    // Test Switch
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    // Test I2C UnitENV III
    unitenv_t unitenv;
    get_i2c_unitenv_data(&unitenv);
    ESP_LOGI(TAG, "%2.1f 度, %2.1f %%, %0.2f hPa", unitenv.tmp, unitenv.hum, unitenv.pressure);
    // Test WebAssembly
    if(enable_wasm) tick_wasm();
    delay(500);
}
