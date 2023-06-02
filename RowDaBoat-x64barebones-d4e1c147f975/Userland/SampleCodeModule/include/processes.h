#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <sysCallInterface.h>
#include <stdio.h>

typedef int pid_t;

int execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[]);
void printProcesses(char argc, char **argv);
int getpid();
void yield();
void exit(int value);
pid_t blockProcess(int pid);
pid_t waitpid(pid_t pid);
pid_t kill(int pid);
void cat();
void wc();
void filter();
void loop(int sec);

#endif