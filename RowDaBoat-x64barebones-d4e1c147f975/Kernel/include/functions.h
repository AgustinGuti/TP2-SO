#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <stdint.h>

void hex_to_str(char * char_num, int num);
void dec_to_str(char * char_num, int num);
int hex_num_length(int num);
int dec_num_length(int num);
char getBit(uint16_t value, int n);

#endif