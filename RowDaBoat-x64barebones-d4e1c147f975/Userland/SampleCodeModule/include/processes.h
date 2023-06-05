#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include <sysCallInterface.h>
#include <stdio.h>
#include <functions.h>

typedef int pid_t;

int execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[]);
char printProcesses(char argc, char **argv);
int getpid();
void yield();
void exit(int value);
pid_t blockProcess(int pid);
pid_t waitpid(pid_t pid);
pid_t kill(int pid);
char cat(char argc, char **argv);
char wc(char argc, char **argv);
char filter(char argc, char **argv);
char loop(char argc, char **argv);
void sleep(int sec);
void setAutoPrio(char argc, char **argv);

#endif