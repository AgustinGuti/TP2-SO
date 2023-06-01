#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <sysCallInterface.h>

typedef int pid_t;

int execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[]);
void printProcesses(char argc, char **argv);
int getpid();
void yield();
void exit(int value);
void blockProcess(int pid);
pid_t waitpid(pid_t pid);
void kill(int pid);
void cat();
void wc();
void filter();
void loop(int sec);

#endif