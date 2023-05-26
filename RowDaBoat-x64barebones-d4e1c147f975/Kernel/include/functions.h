#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdint.h>
#include <lib.h>

void hex_to_str(char *char_num, uint64_t num);
void dec_to_str(char *char_num, int num);
int hex_num_length(uint64_t num);
int dec_num_length(int num);
uint32_t strlen(char *str);
char getBit(uint16_t value, int n);
void strcpy(char *dest, char *src);
int strcmp(const char* str1, const char* str2);
int strToNum(char * str, int longitud);

#endif