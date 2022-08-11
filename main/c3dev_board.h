#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "Adafruit_NeoPixel.h"

#define C3DEV_GPIO_0 0

#define C3DEV_SPI_CLOCK 25000000
#define C3DEV_SPI_SCLK 4
#define C3DEV_SPI_MOSI 5
#define C3DEV_SPI_MISO 6

#define C3DEV_SD_CS 1

#define C3DEV_LCD_CS 7
#define C3DEV_LCD_DC 10
#define C3DEV_LCD_RST 8

#define C3DEV_SW1 9
#define M5STAMP_C3_SW 3
#define M5STAMP_C3_LED 2

extern Adafruit_ST7735 tft;
extern Adafruit_NeoPixel pixels;
extern SPIClass *spi;
