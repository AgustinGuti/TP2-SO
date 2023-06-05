#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

void timer_handler();
uint32_t ticks_elapsed();
uint32_t getMillis();
#endif
