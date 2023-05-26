#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "linkedList.h"
#include "memory.h"
#include "functions.h"
#include "process.h"

#define STACK_SIZE 4096
#define BURST_TIME 10

typedef struct SchedulerCDT *Scheduler;
typedef int pid_t;

void initScheduler();
void *schedule(void *stackPointer);
pid_t execve(void *entryPoint, char *const argv[]);
pid_t fork();
void printProcesses();
pid_t getpid();
void yield();
void exit(int value);
void blockHandler(pid_t pid);
void blockProcess(pid_t pid);
void unblockProcess(pid_t pid);
int getProcessState(pid_t pid);
int nice(pid_t pid, int priority);
void startWrapper(void *entryPoint, char argc, char *argv[]);
pid_t waitpid(pid_t pid);

#endif /* SCHEDULER_H */
