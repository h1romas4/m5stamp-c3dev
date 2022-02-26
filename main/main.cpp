#include "Arduino.h"
#include "esp_log.h"
#include "SPI.h"
#include "SD.h"
#include "SPIFFS.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "tinyPNG.h"

#include "c3dev_board.h"
#include "test_freetype.h"
#include "test_wasm3.h"

static const char *TAG = "main.cpp";

/**
 * SPI member
 */
SPIClass *spi = &SPI;

/**
 * SPIFFS member
 */
fs::SPIFFSFS SPIFFS_WASM;

/**
 * LCD member
 */
Adafruit_ST7735 tft = Adafruit_ST7735(spi, C3DEV_LCD_CS, C3DEV_LCD_DC, C3DEV_LCD_RST);

/**
 * PNG render member
 */
tinyPNG png;
uint16_t png_y = 0;
uint16_t png_base_x = 0;
uint16_t png_base_y = 0;

/**
 * Example Draw PNG image in SD card
 */
void draw_sdcard_png(const char *filename, uint16_t base_x, uint16_t base_y)
{
    // Read PNG image binary
    uint8_t *image;

    SD.begin(C3DEV_SD_CS, *spi, C3DEV_SPI_CLOCK, "/sdcard", 1, false);

    File file = SD.open(filename, "rb");
    size_t file_size = file.size();
    image = (uint8_t *)malloc(sizeof(uint8_t) * file.size());
    if(image == nullptr) {
        ESP_LOGE(TAG, "Memory alloc error");
        return;
    }
    if(file.read(image, file_size) != file.size()) {
        ESP_LOGE(TAG, "SD read error");
        return;
    }
    file.close();

    SD.end();

    // Draw PNG image
    png_y = 0;
    png_base_x = base_x;
    png_base_y = base_y;
    png.setPNG((unsigned char *)image, file_size);
    png.process([](unsigned char *line) {
        int x;
        uint16_t color;
        tft.startWrite();
        for (x = 0; x < png.getWidth(); x++) {
            // https://stackoverflow.com/questions/22937080/32bit-rgba-to-16bit-bgr565-conversion
            color  =  line[(x * 3) + 2] >> 3; // blue
            color |= (line[(x * 3) + 1] & 0xFC) << 3; // green
            color |= (line[(x * 3) + 0]  & 0xF8) << 8; // red
            tft.writePixel(x + png_base_x, png_y + png_base_y, color);
        }
        png_y++;
        tft.endWrite();
    });

    free(image);
}

void setup(void)
{
    // Sharing the SPI bus among SD card and other SPI devices
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/peripherals/sdspi_share.html
    // first step C3DEV_LCD_CS high
    pinMode(C3DEV_LCD_CS, OUTPUT);
    digitalWrite(7, HIGH);

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
    draw_freetype();

    // Test SD card and PNG
    // draw_sdcard_png("/M5STACK/TEST10-0.PNG", 0, 0);
    // draw_sdcard_png("/M5STACK/TEST10-1.PNG", 80, 0);
    // draw_sdcard_png("/M5STACK/TEST10-2.PNG", 0, 60);
    // draw_sdcard_png("/M5STACK/TEST10-3.PNG", 80, 60);

    // Test WebAssembly
    SPIFFS_WASM.begin(false, "/wasm", 4, "wasm");
    File wasm_file = SPIFFS_WASM.open("/app.wasm", "rb");
    size_t wasm_size = wasm_file.size();
    ESP_LOGI(TAG, "app.wasm: %d", wasm_size);
    // Read .wasm
    uint8_t *wasm_binary = (uint8_t *)malloc(sizeof(uint8_t) * wasm_size);
    if(wasm_binary == nullptr) {
        ESP_LOGE(TAG, "Memory alloc error");
    }
    if(wasm_file.read(wasm_binary, wasm_size) != wasm_size) {
        ESP_LOGE(TAG, "SPIFFS read error");
    }
    wasm_file.close();
    SPIFFS_WASM.end();
    // Load WebAssembly
    load_wasm(wasm_binary, wasm_size);
}

void loop(void)
{
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    delay(1000);
}
