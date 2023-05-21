#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdint.h>

void hexToStr(char * char_num, long num);
void decToStr(char * char_num, int num);
int hexNumLength(long num);
int decNumLength(int num);
int strToNum(char * str, int longitud);
uint32_t strlen(char *str);
void buildRectSprite(uint16_t width, uint16_t height, uint32_t color, uint8_t **location);
int isHexaNumber(char * str);
uint64_t hexaStrToNum(char * str, int longitud, char *overflowFlag);    //overflow flag in 1 if there is overflow
int strcmp(const char* str1, const char* str2);

#endif