#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "linkedList.h"
#include "memory.h"
#include "functions.h"
#include "process.h"
#include "time.h"
#include "pipes.h"

#define STACK_SIZE 4096
#define BURST_TIME 50
#define MAX_WAITING_TIME 100

typedef struct ProcessCDT *Process;

typedef struct SchedulerCDT *Scheduler;
typedef int pid_t;

void initScheduler();
void closeScheduler();
void *schedule(void *stackPointer);
pid_t execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[]);
void printProcesses(char showKilled);
pid_t getpid();
void yield();
void exit(int value);
pid_t killProcess(pid_t pid);
void killForegroundProcess();
pid_t blockHandler(pid_t pid);
void blockProcess(pid_t pid);
void unblockProcess(pid_t pid);
int getProcessState(pid_t pid);
int nice(pid_t pid, int priority);
void startWrapper(void *entryPoint, char argc, char *argv[]);
pid_t waitpid(pid_t pid);
Process getCurrentProcess();
void sleep(int millis);

#endif /* SCHEDULER_H */
