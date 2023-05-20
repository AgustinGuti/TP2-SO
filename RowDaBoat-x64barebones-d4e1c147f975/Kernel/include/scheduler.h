#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "linkedList.h"
#include "memory.h"
#include "functions.h"

#define STACK_SIZE 4096

typedef enum {READY, RUNNING, BLOCKED, ZOMBIE} processState;
typedef int pid_t;

typedef struct ProcessCDT * Process;
typedef struct SchedulerCDT * Scheduler;

void initScheduler();
void *schedule(void* stackPointer);
int fork();
int execve(void* entryPoint, char * const argv[]);
pid_t createProcess(char* name, void* entryPoint, uint8_t priority, uint8_t foreground, char * argv[]);
void printProcesses();
pid_t getpid();
void yield();
void exit(int value);
void blockProcess(int pid);
void unblockProcess(int pid);

#endif /* SCHEDULER_H */
