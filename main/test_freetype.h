#include "ffsupport.h"
#include "font_render.h"

void init_freetype();
void drawFreetypeBitmap(int32_t cx, int32_t cy, uint16_t bw, uint16_t bh, uint16_t fg, uint8_t *bitmap);
void drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg, font_render_t *render);
void draw_freetype();
