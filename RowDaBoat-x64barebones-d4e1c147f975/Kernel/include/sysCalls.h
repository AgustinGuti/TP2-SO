#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_

#include <stdarg.h>
#include <videoDriver.h>
#include <keyboard.h>
#include <time.h>
#include <clockDriver.h>
#include <stdint.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void _sysCallHandler();
void setupSysCalls();

#endif