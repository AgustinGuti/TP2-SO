#ifndef _PROCESSES_H_
#define _PROCESSES_H_

int fork();
int execve(void* entryPoint, char * const argv[]);
void printProcesses();

#endif