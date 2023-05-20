#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <sysCallInterface.h>

int fork();
int execve(void* entryPoint, char * const argv[]);
void printProcesses();
int getpid();
void yield();
void exit(int value);

#endif