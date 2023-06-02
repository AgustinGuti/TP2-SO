#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_

#include <stdarg.h>
#include <videoDriver.h>
#include <keyboard.h>
#include <time.h>
#include <clockDriver.h>
#include <stdint.h>

void _sysCallHandler();
void setupSysCalls();

#endif