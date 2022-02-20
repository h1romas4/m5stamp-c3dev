#include "freetype_helper.h"

/**
 * alphaBlend
 *
 * from: M5Stack In_eSPI.cpp
 * https://github.com/m5stack/M5Stack/blob/master/src/utility/In_eSPI.cpp#L5884-L5908
 */
uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc)
{
    if (alpha == 255)
    {
        return fgc;
    }
    else if (alpha == 0)
    {
        return bgc;
    }
    if (fgc == bgc)
    {
        return fgc;
    }
    // For speed use fixed point maths and rounding to permit a power of 2 division
    uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
    uint16_t fgG = ((fgc >> 4) & 0x7E) + 1;
    uint16_t fgB = ((fgc << 1) & 0x3E) + 1;

    uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
    uint16_t bgG = ((bgc >> 4) & 0x7E) + 1;
    uint16_t bgB = ((bgc << 1) & 0x3E) + 1;

    // Shift right 1 to drop rounding bit and shift right 8 to divide by 256
    uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
    uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
    uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

    // Combine RGB565 colours into 16 bits
    // return ((r&0x18) << 11) | ((g&0x30) << 5) | ((b&0x18) << 0); // 2 bit greyscale
    // return ((r&0x1E) << 11) | ((g&0x3C) << 5) | ((b&0x1E) << 0); // 4 bit greyscale
    return (r << 11) | (g << 5) | (b << 0);
}

/**
 * decodeUTF8
 *
 */
uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
{
    uint16_t c = buf[(*index)++];
    //
    // 7 bit Unicode
    if ((c & 0x80) == 0x00)
        return c;

    // 11 bit Unicode
    if (((c & 0xE0) == 0xC0) && (remaining > 1))
        return ((c & 0x1F) << 6) | (buf[(*index)++] & 0x3F);

    // 16 bit Unicode
    if (((c & 0xF0) == 0xE0) && (remaining > 2))
    {
        c = ((c & 0x0F) << 12) | ((buf[(*index)++] & 0x3F) << 6);
        return c | ((buf[(*index)++] & 0x3F));
    }

    // 21 bit Unicode not supported so fall-back to extended ASCII
    // if ((c & 0xF8) == 0xF0) return c;

    return c; // fall-back to extended ASCII
}
