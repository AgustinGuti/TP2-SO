#ifndef _LIB_H
#define _LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);
int32_t readDirection32bits(int dir);
int16_t readDirection16bits(int dir);
#endif