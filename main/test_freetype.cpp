#include "SPIFFS.h"
#include "ffsupport.h"
#include "font_render.h"
#include "freetype_helper.h"

#include "test_freetype.h"
#include "c3dev_board.h"

static const char *TAG = "test_freetype.cpp";

/**
 * SPIFFS member
 */
fs::SPIFFSFS SPIFFS_FONT;

/**
 * FreeType member
 */
font_face_t font_face;
font_render_t font_render;
static const int font_size = 20;
static const int font_cache_size = 64;
const uint8_t alphamap[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

void init_freetype(void)
{
    // FreeType initialize
    SPIFFS_FONT.begin(false, "/font", 4, "font");
    ffsupport_setffs(SPIFFS_FONT);
    if (font_face_init_fs(&font_face, "/GENSHINM.TTF") != ESP_OK) {
        ESP_LOGE(TAG, "Font load faild.");
    }
    // Font render initialize
    if (font_render_init(&font_render, &font_face, font_size, font_cache_size) != ESP_OK) {
        ESP_LOGE(TAG, "Render creation failed.");
    }
}

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
