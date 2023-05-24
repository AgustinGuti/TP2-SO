#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "memory.h"
#include "functions.h"

#define STACK_SIZE 4096

#define MAX_PRIORITY 4

#define KERNEL_PID -2
#define EMPTY_PID -1

typedef enum{ READY, RUNNING, BLOCKED, ZOMBIE } processState;

typedef int pid_t;

typedef struct ProcessCDT
{
    pid_t pid;
    char *name;
    processState state;
    int priority;
    uint64_t *stack;
    uint64_t *stackBase;
    uint64_t *stackPointer;
    uint8_t foreground;
} ProcessCDT;

typedef struct ProcessCDT *Process;

int fork();
Process createProcess(char *name, void *entryPoint, uint8_t priority, uint8_t foreground, char *argv[], void *startWrapper);
void killProcess(pid_t pid);
void emptyProcess();

#endif