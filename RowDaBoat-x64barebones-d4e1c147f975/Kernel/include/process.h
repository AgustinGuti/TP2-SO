#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "memory.h"
#include "functions.h"
#include <semaphores.h>
#include <videoDriver.h>

#define STACK_SIZE 4096
#define PIPE_SIZE 1024
#define MAX_FDS 5

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
    int fds[MAX_FDS];
} ProcessCDT;


typedef struct ProcessCDT *Process;

int fork();
Process createProcess(char *name, void *entryPoint, uint8_t priority, uint8_t foreground, char *argv[], void *startWrapper);
//void killProcess(pid_t pid);
void emptyProcess();
void freeStack(Process process);
Process dupProcess(Process parentProcess);

#endif