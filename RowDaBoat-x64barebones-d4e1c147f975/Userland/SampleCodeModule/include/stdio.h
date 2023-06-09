#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdint.h>
#include <stdarg.h>
#include <functions.h>
#include <sysCallInterface.h>

#define BACKSPACE 0x08
#define NEWLINE 0x0A
#define EOF -1

void printText(char *string);

void putChar(char character);

int getChar();

void newline();

void backspace();

void printf(const char *fmt, ...);
int scanf(const char *fmt, int argQty, ...);
char cleanScreen(char argc, char **argv);

void formatPrint(const char *fmt, uint32_t color, uint16_t row, uint16_t col, int argQty, ...);
void printFormatText(char *str, uint32_t color, uint16_t row, uint16_t col);

#endif