#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdint.h>
#include <stdarg.h>
#include <functions.h>
#include <sysCallInterface.h>

#define BACKSPACE   0x08
#define NEWLINE     0x0A

void printText(char *string);

void putChar(char character);

uint8_t getChar();

void newline();

void clearScreen();

void backspace();
void cleanScreen();

void printf(const char* fmt, ...);
void printerr(const char* fmt, int argQty, ...);
int scanf(const char *fmt, int argQty, ...);
void cleanScreen();

void formatPrint(const char* fmt, uint32_t color, uint16_t row, uint16_t col, int argQty, ...);
void printFormatChar(char character, uint32_t color, uint16_t row, uint16_t col);
void printFormatText( char* str, uint32_t color, uint16_t row, uint16_t col);
void cat();

#endif