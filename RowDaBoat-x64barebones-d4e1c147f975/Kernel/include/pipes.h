#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>
#include "memory.h"
#include "functions.h"
#include <semaphores.h>
#include <videoDriver.h>

#define PIPE_SIZE 1024

typedef struct PipeCDT *Pipe;

Pipe openPipe(char *name);
int closePipe(Pipe pipe);
int readFromPipe(Pipe pipe, char *buffer, int size);
int writeToPipe(Pipe pipe, char *buffer, int size);

#endif