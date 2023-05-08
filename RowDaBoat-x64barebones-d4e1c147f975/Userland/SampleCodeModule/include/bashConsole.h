#ifndef _BASH_CONSOLE_H_
#define _BASH_CONSOLE_H_

#include <tron.h>
#include <displayTime.h>
#include <functions.h>
#include <memoryDump.h>
#include <himnoAlegria.h>
#include <stdio.h>
#include <stdint.h>

#define LINE_INDICATOR "$> "

int startConsole();
char processCommand(uint8_t *str, int length);
extern void invalidOpcode();

#endif