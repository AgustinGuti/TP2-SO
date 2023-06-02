#ifndef _MEMORYDUMP_H_
#define _MEMORYDUMP_H_

#include <stdio.h>
#include <stdint.h>
#include <sysCallInterface.h>
#include <functions.h>

char memoryDump(char argc, char **argv);
char printRegs(char argc, char **argv);
void saveRegs();

#endif /* MEMORYDUMP_H_ */