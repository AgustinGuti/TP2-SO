#ifndef _MEMORYDUMP_H_
#define _MEMORYDUMP_H_

#include <stdio.h>
#include <stdint.h>
#include <sysCallInterface.h>

void memoryDump(uint64_t direction);
void printRegs();
void saveRegs();

#endif /* MEMORYDUMP_H_ */