#include "Arduino.h"
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
 * UNITENV member
 */
#define UNITENV_STRLEN 64

unitenv_t unitenv;
typedef struct {
    char tmp[UNITENV_STRLEN];
    char hum[UNITENV_STRLEN];
    char pressure[UNITENV_STRLEN];
} unitenv_srt_t;
unitenv_srt_t unitenv_before;
unitenv_srt_t unitenv_now;

void draw_unitenv(void)
{
    get_i2c_unitenv_data(&unitenv);
    snprintf(unitenv_now.tmp, UNITENV_STRLEN, "%2.1f 度", unitenv.tmp);
    snprintf(unitenv_now.hum, UNITENV_STRLEN, "%2.1f %%", unitenv.hum);
    snprintf(unitenv_now.pressure, UNITENV_STRLEN, "%0.2f hPa", unitenv.pressure);
    if(strncmp(unitenv_now.tmp, unitenv_before.tmp, UNITENV_STRLEN) != 0) {
        tft.fillRect(54, 28 * 2 - 15, 106, 16, ST77XX_BLACK);
        draw_freetype_string(unitenv_now.tmp, 54, 28 * 2, ST77XX_WHITE, &font_render_16px);
        strncpy(unitenv_before.tmp, unitenv_now.tmp, UNITENV_STRLEN);
    }
    if(strncmp(unitenv_now.hum, unitenv_before.hum, UNITENV_STRLEN) != 0) {
        tft.fillRect(54, 28 * 3 - 15, 106, 16, ST77XX_BLACK);
        draw_freetype_string(unitenv_now.hum, 54, 28 * 3, ST77XX_WHITE, &font_render_16px);
        strncpy(unitenv_before.hum, unitenv_now.hum, UNITENV_STRLEN);
    }
    if(strncmp(unitenv_now.pressure, unitenv_before.pressure, UNITENV_STRLEN) != 0) {
        tft.fillRect(54, 28 * 4 - 15, 106, 16, ST77XX_BLACK);
        draw_freetype_string(unitenv_now.pressure, 54, 28 * 4, ST77XX_WHITE, &font_render_16px);
        strncpy(unitenv_before.pressure, unitenv_now.pressure, UNITENV_STRLEN);
    }
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
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    // Test FreeType
    init_freetype();
    font_render_20px = create_freetype_render(/* font size */ 20, /* font cache */ 64);

    draw_freetype_string("M5Stamp C3", 10, 28, ST77XX_RED, &font_render_20px);
    draw_freetype_string("Development", 10, 28 * 2, ST77XX_WHITE, &font_render_20px);
    draw_freetype_string("Board", 10, 28 * 3, ST77XX_WHITE, &font_render_20px);
    draw_freetype_string("RISC-V", 10, 28 * 4, ST77XX_BLUE, &font_render_20px);

    // Test SD card and PNG
    // draw_sdcard_png("/M5STACK/TEST10-0.PNG", 0, 0);
    // draw_sdcard_png("/M5STACK/TEST10-1.PNG", 80, 0);
    // draw_sdcard_png("/M5STACK/TEST10-2.PNG", 0, 60);
    // draw_sdcard_png("/M5STACK/TEST10-3.PNG", 80, 60);

    // Test WebAssembly
    exec_wasm();

    // Test I2C (UNITENV sensor) on disabled JTAG GPIO 18/19
    delay(500);
    tft.fillScreen(ST77XX_BLACK);
    init_i2c_gpio1819();
    // Draw screen
    font_render_16px = create_freetype_render(/* font size */ 16, /* font cache */ 64);
    draw_freetype_string("ENV.III SENSOR", 8, 24, ST77XX_RED, &font_render_20px);
    draw_freetype_string("温度:" , 8, 28 * 2, ST77XX_WHITE, &font_render_16px);
    draw_freetype_string("湿度:" , 8, 28 * 3, ST77XX_WHITE, &font_render_16px);
    draw_freetype_string("気圧:" , 8, 28 * 4, ST77XX_WHITE, &font_render_16px);
}

void loop(void)
{
    // Test I2C (UNITENV sensor)
    draw_unitenv();
    // Test SW
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    delay(1000);
}
