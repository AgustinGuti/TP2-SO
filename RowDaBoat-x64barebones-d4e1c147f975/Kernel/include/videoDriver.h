#ifndef _VIDEO_DRIVER_H_
#define _VIDEO_DRIVER_H_

#include <stdint.h>
#include <fonts.h>
#include <stdarg.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BACKSPACE 0x08
#define NEWLINE 0x0A

typedef struct
{
    uint16_t x;
    uint16_t y;
} pxlCoord;

// VBE attributes
uint16_t getScreenWidth();
uint16_t getScreenHeight();
uint16_t getScreenPitch();
uint32_t getScreenFrameBuffer();
uint8_t getScreenBpp();

uint32_t getPixelOffset(pxlCoord coord);

void clearScreen();
void drawLine(pxlCoord initial, pxlCoord final, uint32_t color);
void drawRect(pxlCoord coord, uint32_t color, uint16_t width, uint16_t height);

void printStringLimited(uint32_t color, uint8_t *str, uint32_t count);
void printString(uint32_t color, uint8_t *str);
void putChar(uint32_t color, uint16_t id);

void printFormatStringLimited(uint32_t color, const char *str, uint32_t count, uint16_t row, uint16_t col);

void printf(const char *fmt, ...);
void printerr(const char *fmt, ...);

char getBit(uint16_t value, int n);

void setFontSize(uint8_t size, uint8_t rewrite);
uint8_t getFontSize();

void newline();
void backspace();
void cleanScreen();
void clearCharBuffer();
void startPage();
void sendEOF();

/*
    Draws draws width * height * bpp/8 bytes from sprite to the screen, begining in coord.
    Width and height should be in pizel size
*/
void drawSprite(uint16_t width, uint16_t height, uint8_t sprite[height][width * 3], pxlCoord coord);

#endif