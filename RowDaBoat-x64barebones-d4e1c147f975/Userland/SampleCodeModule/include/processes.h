#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <sysCallInterface.h>

typedef int pid_t;

int fork();
int execve(void* entryPoint, char * const argv[]);
void printProcesses();
int getpid();
void yield();
void exit(int value);
void blockProcess(int pid);
pid_t waitpid(pid_t pid);

#endif