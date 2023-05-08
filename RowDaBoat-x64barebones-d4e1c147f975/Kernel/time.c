#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	ticks++;
}

uint32_t ticks_elapsed() {
	return ticks;
}

uint32_t seconds_elapsed() {
	return ticks / 18;
}

uint32_t getMillis(){
	return ticks * 55;
}