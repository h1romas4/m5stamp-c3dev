#include "Arduino.h"
#include "SD.h"
#include "esp_log.h"
#include "tinyPNG.h"

#include "c3dev_board.h"

static const char *TAG = "test_tinypng.cpp";

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
