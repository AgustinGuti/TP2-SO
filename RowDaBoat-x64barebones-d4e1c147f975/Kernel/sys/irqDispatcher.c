// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <stdint.h>
#include <videoDriver.h>
#include <scheduler.h>

static void int_20(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
static void int_21(uint64_t event, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

typedef void (*InterruptHandler)(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

// Accepts up to 5 parameters
static InterruptHandler interrupts[] = {&int_20, &int_21};

void irqDispatcher(uint64_t irq, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	(*interrupts[irq])(rsi, rdx, rcx, r8, r9);
}

void int_20(uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	timer_handler();
}

void int_21(uint64_t event, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	keyboard_handler(event);
}