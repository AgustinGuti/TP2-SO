#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdint.h>
#include <lib.h>

void hexToStr(char *char_num, uint64_t num);
void decToStr(char *char_num, int num);
int hexNumLength(uint64_t num);
int decNumLength(int num);
uint32_t strlen(const char *str);
char getBit(uint16_t value, int n);
void strcpy(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
int strToNum(const char *str, int longitud);
int intComparator(void *a, void *b);
void triggerTimer();

#endif