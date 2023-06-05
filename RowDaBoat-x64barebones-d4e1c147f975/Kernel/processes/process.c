// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "process.h"

enum
{
    RAX,
    RBX,
    RCX,
    RDX,
    RBP,
    RDI,
    RSI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15
};

#define WHITE 0x00FFFFFF
#define GREEN 0x0000FF00

static pid_t currentPID = KERNEL_PID; // Static variable to track the current PID
pid_t generatePID();
void pushToStack(Process process, uint64_t value);

Process initProcess(char *name, uint8_t priority, uint8_t foreground, pid_t parentPID)
{
    Process process = (Process)malloc(sizeof(ProcessCDT));
    if (process == NULL)
    {
        /*not enough memory to allocate process*/
        return NULL;
    }
    process->hasStack = 1;
    process->pid = generatePID();
    process->name = (char *)malloc(strlen(name) + 1);
    process->exitValue = 0;
    if (process->name == NULL)
    {
        /*not enough memory for process->name*/
        free(process);
        return NULL;
    }
    strcpy(process->name, name);
    process->priority = priority - 1;
    process->foreground = foreground;
    process->parentPID = parentPID;
    process->waitingForPID = -1;
    process->waitingSem = NULL;
    process->sleepTime = 0;
    process->waitingTime = 0;
    process->fds = (Pipe *)malloc(INITIAL_FD_LIMIT * sizeof(Pipe));
    if (process->fds == NULL)
    {
        /*not enough memory for process->fds*/
        free(process->name);
        free(process);
        return NULL;
    }
    process->fdLimit = INITIAL_FD_LIMIT;
    process->pipeTypes = (PipeType *)malloc(INITIAL_FD_LIMIT * sizeof(PipeType));
    if (process->pipeTypes == NULL)
    {
        /*not enough memory for process->pipeTypes*/
        free(process->name);
        free(process->fds);
        free(process);
        return NULL;
    }
    // Process stack is the top of the stack, stack base is process->stack + STACK_SIZE
    process->stack = (uint64_t *)malloc(STACK_SIZE * sizeof(process->stack[0]));

    if (process->stack == NULL)
    {
        /*not enough memory for process->stack*/
        free(process->name);
        free(process);
        printf("not enough memory for process->stack\n");
        return NULL;
    }
    process->stackBase = process->stack + STACK_SIZE;
    process->stackPointer = process->stackBase;

    return process;
}

Process createProcess(char *name, void *entryPoint, uint8_t priority, uint8_t foreground, char *argv[], void *startWrapper, pid_t parentPID, Pipe *pipes, char pipeQty)
{
    Process process = initProcess(name, priority, foreground, parentPID);
    if (process == NULL)
    {
        /*not enough memory to allocate process*/
        return NULL;
    }
    process->stdio = getKeyboardBuffer();
    process->fds[STDIN] = process->stdio;
    process->fds[STDOUT] = process->stdio;
    process->pipeTypes[STDIN] = READ;
    process->pipeTypes[STDOUT] = WRITE;

    Process parent = getProcess(parentPID);
    if (parent != NULL)
    {
        if (parent->fds[STDIN] != NULL)
        {
            process->fds[STDIN] = parent->fds[STDIN];
            process->pipeTypes[STDIN] = parent->pipeTypes[STDIN];
        }
        if (parent->fds[STDOUT] != NULL)
        {
            process->fds[STDOUT] = parent->fds[STDOUT];
            process->pipeTypes[STDOUT] = parent->pipeTypes[STDOUT];
        }
    }

    if (pipeQty > 0)
    {
        if (pipes[0] != NULL)
        {
            process->fds[STDIN] = pipes[0];
        }
    }
    if (pipeQty > 1)
    {
        if (pipes[1] != NULL)
        {
            process->fds[STDOUT] = pipes[1];
        }
    }
    int j = 0;
    for (j = 2; j < pipeQty; j++)
    {
        if (j > process->fdLimit)
        {
            process->fdLimit *= 2;
            Pipe *auxFds = realloc(process->fds, process->fdLimit * sizeof(Pipe));
            PipeType *auxPipeTypes = realloc(process->pipeTypes, process->fdLimit * sizeof(PipeType));
            if (auxFds == NULL || auxPipeTypes == NULL)
            {
                /*not enough memory for process->fds or process->pipeTypes*/
                free(process->name);
                free(process->fds);
                free(process->pipeTypes);
                free(process);
                free(auxFds);
                free(auxPipeTypes);
                return NULL;
            }
            process->fds = auxFds;
            process->pipeTypes = auxPipeTypes;
        }
        process->fds[j] = pipes[j];
        process->pipeTypes[j] = j % 2 == 0 ? READ : WRITE;
    }

    for (int i = j; i < process->fdLimit; i++)
    {
        process->fds[i] = NULL;
    }
    for (int i = j; i < process->fdLimit; i++)
    {
        process->pipeTypes[i] = EMPTY;
    }

    int argc = 0;
    if (argv != NULL)
    {
        while (argv[argc] != NULL)
        {
            argc++;
        }
    }

    char **argvAux = NULL;
    if (argc > 0)
    {
        argvAux = (char **)malloc((argc) * sizeof(char *));
        if (argvAux == NULL)
        {
            /*not enough memory for argvAux*/
            free(process->name);
            free(process->fds);
            free(process->pipeTypes);
            free(process);
            return NULL;
        }
    }
    process->argv = argvAux;
    process->argc = argc;

    for (int i = 0; i < argc; i++)
    {
        argvAux[i] = (char *)malloc(strlen(argv[i]) + 1);
        if (argvAux[i] == NULL)
        {
            /*not enough memory for argvAux[i]*/
            free(process->name);
            free(process->fds);
            free(process->pipeTypes);
            free(process);
            return NULL;
        }
        strcpy(argvAux[i], argv[i]);
    }
    // argvAux[argc] = NULL;

    pushToStack(process, 0x0);                          // ss
    pushToStack(process, (uint64_t)process->stackBase); // stackPointer
    pushToStack(process, 0x202);                        // rflags
    pushToStack(process, 0x8);                          // cs
    pushToStack(process, (uint64_t)startWrapper);       // rip

    for (int i = 0; i < 15; i++)
    { // push 15 registries
        if (i == RDI)
        {
            pushToStack(process, (uint64_t)entryPoint);
        }
        else if (i == RSI)
        {
            pushToStack(process, argc);
        }
        else if (i == RDX)
        {
            pushToStack(process, (uint64_t)argvAux);
        }
        else
        {
            pushToStack(process, 0x0);
        }
    }
    if (entryPoint == NULL)
    {
        process->state = BLOCKED;
    }
    else
    {
        process->state = READY;
    }

    return process;
}

void restartProcessPID()
{
    currentPID = KERNEL_PID;
}

Pipe openProcessPipe(char *name, int fds[2])
{
    Process process = getCurrentProcess();
    Pipe pipe = openPipe(name);
    if (pipe == NULL)
    {
        return NULL;
    }
    if (fds == NULL)
    {
        return pipe;
    }
    int fd = 0;
    while (process->fds[fd] != NULL)
    {
        if (fd == process->fdLimit)
        {
            process->fdLimit *= 2;
            Pipe *auxFds = realloc(process->fds, process->fdLimit * sizeof(Pipe));
            PipeType *auxPipeTypes = realloc(process->pipeTypes, process->fdLimit * sizeof(PipeType));
            if (auxFds == NULL || auxPipeTypes == NULL)
            {
                /*not enough memory for process->fds or process->pipeTypes*/
                free(process->name);
                free(process->fds);
                free(process->pipeTypes);
                free(process);
                free(auxFds);
                free(auxPipeTypes);
                return NULL;
            }
            process->fds = auxFds;
            process->pipeTypes = auxPipeTypes;
        }
        fd++;
    }
    process->fds[fd] = pipe;
    fds[0] = fd;
    fd++;
    while (process->fds[fd] != NULL)
    {
        if (fd == process->fdLimit)
        {
            process->fdLimit *= 2;
            Pipe *auxFds = realloc(process->fds, process->fdLimit * sizeof(Pipe));
            PipeType *auxPipeTypes = realloc(process->pipeTypes, process->fdLimit * sizeof(PipeType));
            if (auxFds == NULL || auxPipeTypes == NULL)
            {
                /*not enough memory for process->fds or process->pipeTypes*/
                free(process->name);
                free(process->fds);
                free(process->pipeTypes);
                free(process);
                free(auxFds);
                free(auxPipeTypes);
                return NULL;
            }
            process->fds = auxFds;
            process->pipeTypes = auxPipeTypes;
        }
        fd++;
    }
    process->fds[fd] = pipe;
    fds[1] = fd;
    return pipe;
}

int closeProcessPipe(int fd)
{
    Process process = getCurrentProcess();
    if (fd < 0 || fd >= process->fdLimit)
    {
        return -1;
    }
    if (process->fds[fd] == NULL)
    {
        return -1;
    }
    closePipe(process->fds[fd]);
    process->fds[fd] = NULL;
    return 0;
}

int readProcessPipe(int fd, char *buffer, int size)
{
    Process process = getCurrentProcess();
    if (fd < 0 || fd >= process->fdLimit)
    {
        return -1;
    }
    if (process->fds[fd] == NULL || process->pipeTypes[fd] != READ)
    {
        return -1;
    }
    if (process->fds[fd] == process->stdio && !process->foreground)
    {
        return 0;
    }
    return readFromPipe(process->fds[fd], buffer, size);
}

int writeProcessPipe(int fd, char *buffer, int size)
{

    Process process = getCurrentProcess();
    if (fd < 0 || fd >= process->fdLimit)
    {
        return -1;
    }
    if (process->fds[fd] == NULL || process->pipeTypes[fd] != WRITE)
    {
        return -1;
    }
    if (process->fds[fd] == process->stdio)
    {
        int index = 0;
        while (index < size && buffer[index] != 0 && buffer[0] != -1)
        {
            if (buffer[index] == -1)
            {
                writeToPipe(process->fds[fd], &buffer[index], 1);
                index++;
                break;
            }
            putChar(WHITE, buffer[index]);
            index++;
        }
        return size;
    }
    return writeToPipe(process->fds[fd], buffer, size);
}

void freeStack(Process process)
{
    if (process != NULL && process->hasStack == 1)
    {
        free(process->stack);
        process->hasStack = 0;
    }
}

pid_t generatePID()
{
    return currentPID++; // Increment and return the new PID
}

void pushToStack(Process process, uint64_t value)
{
    *(--process->stackPointer) = value;
}

uint64_t popFromStack(Process process)
{
    return *(process->stackPointer++);
}

void emptyProcess()
{
    while (1)
    {
        _hlt();
    }
}

void deleteProcess(Process process)
{
    if (process == NULL)
    {
        return;
    }
    freeStack(process);
    free(process->name);
    closePipes(process);
    free(process->fds);
    free(process->pipeTypes);
    if (process->argv != NULL)
    {
        for (int i = 0; i < process->argc; i++)
        {
            free(process->argv[i]);
        }
        free(process->argv);
    }
    free(process);
    process = NULL;
}

void closePipes(Process process)
{
    for (int i = 0; i < process->fdLimit; i++)
    {
        if (process->fds[i] != process->stdio)
        {
            closePipe(process->fds[i]);
        }
    }
}

int getProcessState(Process process)
{
    if (process == NULL)
    {
        return -1;
    }
    return process->state;
}