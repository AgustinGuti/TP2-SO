#ifndef _TESTPROCESSES_H_
#define _TESTPROCESSES_H_

#include <stdint.h>
#include <stddef.h>
#include <processes.h>
char testProcesses(char argc, char *argv[]);
void readerProcess();
void writerProcess();
#endif