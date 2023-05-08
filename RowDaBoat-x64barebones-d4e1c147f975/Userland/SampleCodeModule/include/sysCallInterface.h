#ifndef _SYS_CALL_INTERFACE_H_
#define _SYS_CALL_INTERFACE_H_

#include <stdint.h>

#define STDERR 2
#define STDOUT 1
#define STDIN 0

void _sys_write(int fd, char *str, int length);
void _sys_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint32_t color);
uint32_t _sys_getMillis();
int _sys_read(int fd, uint16_t *buf, int count);
uint16_t _sys_getScreenWidth();
uint16_t _sys_getScreenHeight();
void _sys_cleanScreen();
void _sys_beep(uint16_t frequency);
void _sys_getTime(int* hour, int* min, int* seg);
void _sys_setFontSize(uint8_t size, uint8_t rewrite);
uint8_t _sys_getFontSize();
uint8_t _sys_getScreenBpp();
char _sys_getSavedRegisters(uint64_t registers[17]);
void _sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color,uint16_t row, uint16_t col);
void _sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width*_sys_getScreenBpp()/8]);

#endif