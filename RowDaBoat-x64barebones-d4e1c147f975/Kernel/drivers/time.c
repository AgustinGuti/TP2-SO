// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>

static unsigned long ticks = 0;

void timer_handler()
{
	ticks++;
}

uint32_t ticks_elapsed()
{
	return ticks;
}

uint32_t getMillis()
{
	return ticks * 55;
}