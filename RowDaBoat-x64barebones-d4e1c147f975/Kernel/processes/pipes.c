// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pipes.h>

#define EOF -1

typedef struct PipeCDT
{
    char *buffer;
    char *name;
    int size;
    int readIndex;
    int writeIndex;
    int attached;
    int blocked;
    sem_t sem;
    sem_t mutex;
} PipeCDT;

typedef struct PipesCDT
{
    LinkedList pipes;
    Iterator it;
} PipesCDT;

typedef struct PipesCDT *Pipes;

static Pipes pipes = NULL;

Pipe openPipe(char *name)
{
    if (name != NULL && strlen(name) == 0)
    {
        return NULL;
    }
    if (pipes == NULL)
    {
        pipes = malloc(sizeof(struct PipesCDT));
        pipes->pipes = createLinkedList();
        pipes->it = iterator(pipes->pipes);
    }
    char *newName = NULL;
    if (name != NULL)
    {
        resetIterator(pipes->it);
        while (hasNext(pipes->it))
        {
            Pipe pipe = next(pipes->it);
            if (strcmp(pipe->name, name) == 0)
            {
                pipe->attached++;
                return 0;
            }
        }
        newName = (char *)malloc(strlen(name) + 1);
        if (newName == NULL)
        {
            return NULL;
        }
        strcpy(newName, name);
    }

    Pipe pipe = (Pipe)malloc(sizeof(PipeCDT));
    if (pipe == NULL)
    {
        return NULL;
    }
    pipe->buffer = (char *)malloc(PIPE_SIZE * sizeof(pipe->buffer[0]));
    if (pipe->buffer == NULL)
    {
        return NULL;
    }
    pipe->name = newName;
    pipe->size = PIPE_SIZE;
    pipe->readIndex = 0;
    pipe->writeIndex = 0;
    pipe->attached = 1;
    pipe->blocked = 0;
    pipe->sem = semOpen(NULL, 0);
    pipe->mutex = semOpen(NULL, 1);
    if (pipe->sem == NULL)
    {
        return NULL;
    }
    if (newName != NULL)
    {
        insert(pipes->pipes, pipe);
    }
    return pipe;
}

int closePipe(Pipe pipe)
{
    if (pipe == NULL)
    {
        return -1;
    }
    if (pipe->attached > 1)
    {
        pipe->attached--;
    }
    else
    {
        semClose(pipe->sem);
        semClose(pipe->mutex);
        free(pipe->buffer);
        if (pipe->name != NULL)
        {
            remove(pipes->pipes, pipe);
            free(pipe->name);
        }
        free(pipe);
    }
    if (getSize(pipes->pipes) == 0){
        destroyLinkedList(pipes->pipes);
        freeIterator(pipes->it);
        free(pipes);
        pipes = NULL;
    }
    return 0;
}

int readFromPipe(Pipe pipe, char *buffer, int size)
{
    if (pipe == NULL || buffer == NULL || size < 0)
    {
        return -1;
    }
    int i = 0;
    while (i < size)
    {
        if (pipe->readIndex == pipe->writeIndex)
        {
            semWait(pipe->mutex);
            pipe->blocked++;
            semPost(pipe->mutex);
            semWait(pipe->sem);
        }
        semWait(pipe->mutex);
        buffer[i] = pipe->buffer[pipe->readIndex];
        pipe->readIndex = (pipe->readIndex + 1) % pipe->size;
        i++;
        if (buffer[i - 1] == EOF)
        {
            semPost(pipe->mutex);
            return i;
        }
        semPost(pipe->mutex);
    }
    return i;
}

int writeToPipe(Pipe pipe, char *buffer, int size)
{
    if (pipe == NULL || buffer == NULL || size < 0)
    {
        return -1;
    }
    int i = 0;
    while (i < size)
    {
        semWait(pipe->mutex);
        pipe->buffer[pipe->writeIndex] = buffer[i];
        pipe->writeIndex = (pipe->writeIndex + 1) % pipe->size;
        i++;
        if (pipe->readIndex == (pipe->writeIndex - 1) % pipe->size)
        {
            if (pipe->blocked)
            {
                pipe->blocked--;
                semPost(pipe->sem);
            }
        }
        semPost(pipe->mutex);
    }
    return i;
}