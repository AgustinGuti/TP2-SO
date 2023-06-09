#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdint.h>

void hexToStr(char *char_num, long num);
void decToStr(char *char_num, int num);
int hexNumLength(long num);
int decNumLength(int num);
int strToNum(const char *str, int longitud);
uint32_t strlen(const char *str);
void buildRectSprite(uint16_t width, uint16_t height, uint32_t color, uint8_t **location);
int isHexaNumber(const char *str);
uint64_t hexaStrToNum(const char *str, int longitud, char *overflowFlag); // overflow flag in 1 if there is overflow
int strcmp(const char *str1, const char *str2);
void *memcpy(void *destination, const void *source, uint64_t length);
void strcpy(char *dest, const char *src);

#endif