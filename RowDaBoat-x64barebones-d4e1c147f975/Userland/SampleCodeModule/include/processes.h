#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <sysCallInterface.h>

typedef int pid_t;

int execve(void* entryPoint, Pipe* pipes, char pipeQty, char * const argv[]);
void printProcesses();
int getpid();
void yield();
void exit(int value);
void blockProcess(int pid);
pid_t waitpid(pid_t pid);

#endif