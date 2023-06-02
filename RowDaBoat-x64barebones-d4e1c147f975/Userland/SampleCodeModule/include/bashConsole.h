#ifndef _BASH_CONSOLE_H_
#define _BASH_CONSOLE_H_

#include <displayTime.h>
#include <functions.h>
#include <memoryDump.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <processes.h>
#include <testMM.h>
#include <testSync.h>
#include <testProcesses.h>
#include <testPrio.h>
#include <memory.h>
#include <phylos.h>
#include <pipes.h>

#define LINE_INDICATOR "$> "

int startConsole();
char processCommand(uint8_t *str, int length);
extern char invalidOpcode(char argc, char **argv);
extern char zeroDivision(char argc, char **argv);

#endif