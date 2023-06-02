#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "memory.h"
#include "functions.h"
#include <semaphores.h>
#include <videoDriver.h>
#include <keyboard.h>
#include "pipes.h"

// Forward declaration for Pipe type
typedef struct PipeCDT PipeCDT;
typedef PipeCDT *Pipe;

#define STACK_SIZE 4096
#define PIPE_SIZE 1024
#define INITIAL_FD_LIMIT 10

#define MAX_PRIORITY 4

#define KERNEL_PID -2
#define EMPTY_PID -1

typedef enum
{
    READY,
    RUNNING,
    BLOCKED,
    ZOMBIE
} processState;

typedef int pid_t;

typedef enum
{
    READ,
    WRITE,
    EMPTY
} PipeType;

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
    Pipe *fds;
    PipeType *pipeTypes;
    Pipe stdio;
    int fdLimit;
    pid_t parentPID;
    pid_t waitingForPID;
    sem_t waitingSem;
    int sleepTime;
    uint64_t waitingTime;
    char **argv;
    char argc;
} ProcessCDT;

typedef struct ProcessCDT *Process;

Process createProcess(char *name, void *entryPoint, uint8_t priority, uint8_t foreground, char *argv[], void *startWrapper, pid_t parentPID, Pipe *pipes, char pipeQty);
// void killProcess(pid_t pid);
void emptyProcess();
void freeStack(Process process);
Pipe openProcessPipe(char *name, int fds[2]);
int closeProcessPipe(int fd);
int readProcessPipe(int fd, char *buffer, int bytes);
int writeProcessPipe(int fd, char *buffer, int size);
void deleteProcess(Process process);
void closePipes(Process process);

#endif