#include <time.h>
#include <stdint.h>
#include <videoDriver.h>

static void int_20();
static void int_21(uint8_t event);
										//Accepts up to 5 parameters
static const uint64_t (*interrupts[])(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) = {&int_20,&int_21};

void irqDispatcher(uint64_t irq, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
	(*interrupts[irq])(rsi,rdx,rcx,r8,r9);
}

void int_20() {
	timer_handler();
}

void int_21(uint8_t event){
	keyboard_handler(event);
}