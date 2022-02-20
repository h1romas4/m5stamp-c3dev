#include "Arduino.h"
#include "esp_log.h"
#include "SPI.h"
#include "SD.h"
#include "SPIFFS.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "ffsupport.h"
#include "font_render.h"
#include "freetype_helper.h"
#include "tinyPNG.h"

#include "c3dev_board.h"

static const char *TAG = "c3dev";

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
font_face_t font_face;
font_render_t font_render;
static const int font_size = 20;
static const int font_cache_size = 64;
const uint8_t alphamap[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

/**
 * PNG render member
 */
tinyPNG png;
uint16_t png_y = 0;
uint16_t png_base_x = 0;
uint16_t png_base_y = 0;

/**
 * Utility drawFreetypeBitmap
 */
void drawFreetypeBitmap(int32_t cx, int32_t cy, uint16_t bw, uint16_t bh, uint16_t fg, uint8_t *bitmap)
{
    uint32_t pos = 0;
    uint16_t bg = 0;
    for (int y = 0; y < bh; y++) {
        for (int x = 0; x < bw; x++) {
            if (pos & 0x1) {
                tft.drawPixel(cx + x, cy + y, alphaBlend(alphamap[bitmap[pos >> 1] & 0x0F], fg, bg));
            } else {
                tft.drawPixel(cx + x, cy + y, alphaBlend(alphamap[bitmap[pos >> 1] >> 4], fg, bg));
            }
            pos++;
        }
    }
}

/**
 * Utility drawString
 */
void drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg, font_render_t *render)
{
    int16_t sumX = 0;
    uint16_t len = strlen(string);
    uint16_t n = 0;
    uint16_t base_y = poY;

    while (n < len) {
        uint16_t uniCode = decodeUTF8((uint8_t *)string, &n, len - n);
        if (font_render_glyph(render, uniCode) != ESP_OK) {
            ESP_LOGE(TAG, "Font render faild.");
        }
        drawFreetypeBitmap(poX + render->bitmap_left,
            base_y - render->bitmap_top,
            render->bitmap_width,
            render->bitmap_height,
            fg,
            render->bitmap
        );
        poX += render->advance;
        sumX += render->advance;
    }
}

/**
 * Example Draw FreeType
 */
void draw_freetype()
{
    drawString("M5Stamp C3", 10, 28, ST77XX_RED, &font_render);
    drawString("Development", 10, 28 * 2, ST77XX_WHITE, &font_render);
    drawString("Board", 10, 28 * 3, ST77XX_WHITE, &font_render);
    drawString("RISC-V", 10, 28 * 4, ST77XX_BLUE, &font_render);
}

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

    // FreeType initialize
    SPIFFS.begin(false, "/spiffs", 4, "font");
    ffsupport_setffs(SPIFFS);
    if (font_face_init_fs(&font_face, "/GENSHINM.TTF") != ESP_OK) {
        ESP_LOGE(TAG, "Font load faild.");
    }
    // Font render initialize
    if (font_render_init(&font_render, &font_face, font_size, font_cache_size) != ESP_OK) {
        ESP_LOGE(TAG, "Render creation failed.");
    }

    // LCD initialize
    tft.initR(INITR_BLACKTAB);
    tft.setSPISpeed(C3DEV_SPI_CLOCK);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    // Test FreeType
    draw_freetype();

    // Test SD card and PNG
    // draw_sdcard_png("/M5STACK/TEST10-0.PNG", 0, 0);
    // draw_sdcard_png("/M5STACK/TEST10-1.PNG", 80, 0);
    // draw_sdcard_png("/M5STACK/TEST10-2.PNG", 0, 60);
    // draw_sdcard_png("/M5STACK/TEST10-3.PNG", 80, 60);
}

void loop(void)
{
    ESP_LOGI(TAG, "SW: %d, SW1: %d", digitalRead(M5STAMP_C3_SW), digitalRead(C3DEV_SW1));
    delay(1000);
}
