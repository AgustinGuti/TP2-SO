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


Process initProcess(char *name, uint8_t priority, uint8_t foreground, pid_t parentPID){
    Process process = (Process)malloc(sizeof(ProcessCDT));
    if (process == NULL)
    {
        /*not enough memory to allocate process*/
        return NULL;
    }
    process->pid = generatePID();
    process->name = (char *)malloc(strlen(name) + 1);
    if (process->name == NULL)
    {
        /*not enough memory for process->name*/
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
    process->fds = (Pipe *)malloc(INITIAL_FD_LIMIT*sizeof(Pipe));
    process->fdLimit = INITIAL_FD_LIMIT;
    process->pipeTypes = (PipeType *)malloc(INITIAL_FD_LIMIT*sizeof(PipeType));

    // Process stack is the top of the stack, stack base is process->stack + STACK_SIZE
    process->stack = (uint64_t *)malloc(STACK_SIZE*sizeof(process->stack[0]));
    //memset(process->stack, 0, STACK_SIZE);
    if (process->stack == NULL)
    {
        /*not enough memory for process->stack*/
        return NULL;
    }
    process->stackBase = process->stack + STACK_SIZE;
    process->stackPointer = process->stackBase;
    process->fds[STDIN] = STDIN;
    process->fds[STDOUT] = STDOUT;
    for(int i = 2; i < process->fdLimit; i++){
        process->fds[i] = -1;
    }
    return process;
}

Process createProcess(char *name, void *entryPoint, uint8_t priority, uint8_t foreground, char *argv[], void *startWrapper, pid_t parentPID, Pipe* pipes, char pipeQty)
{
    Process process = initProcess(name, priority, foreground, parentPID);
    //Pipe stdPipe = createPipe(NULL);
    process->stdio = getKeyboardBuffer();
    process->fds[STDIN] = process->stdio;
    process->fds[STDOUT] = process->stdio;
    process->pipeTypes[STDIN] = READ;
    process->pipeTypes[STDOUT] = WRITE;

    if (pipeQty > 0){
        if (pipes[0] != NULL){
            process->fds[STDIN] = pipes[0];
        }
    }
    if (pipeQty > 1){
        if (pipes[1] != NULL){
            process->fds[STDOUT] = pipes[1];
        }
    }
    int j = 0;
    for (j = 2; j < pipeQty; j++)
    {
        if (j > process->fdLimit)
        {
            return -10;
            // process->fds = (Pipe *)realloc(process->fds, process->fdLimit * 2 * sizeof(Pipe));
            // process->pipeTypes = (PipeType *)realloc(process->pipeTypes, process->fdLimit * 2 * sizeof(PipeType));
            // process->fdLimit *= 2;
        }
        process->fds[j] = pipes[j];
        process->fds[j] = pipes[j];
        process->pipeTypes[j] = j % 2 == 0 ? READ : WRITE;
    }

    for(int i = j; i < process->fdLimit; i++){
        process->fds[i] = NULL;
    }
    for(int i = j; i < process->fdLimit; i++){
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
    char **argvAux = (char **)malloc((argc + 1) * sizeof(char *));
    if (argvAux == NULL)
    {
        /*not enough memory for argvAux*/
        return NULL;
    }
    for (int i = 0; i < argc; i++)
    {
        argvAux[i] = (char *)malloc(strlen(argv[i]) + 1);
        if (argvAux[i] == NULL)
        {
            /*not enough memory for argvAux[i]*/
            return NULL;
        }
        strcpy(argvAux[i], argv[i]);
    }
    argvAux[argc] = NULL;    

    pushToStack(process, 0x0);                // ss
    pushToStack(process, process->stackBase); // stackPointer
    pushToStack(process, 0x202);              // rflags
    pushToStack(process, 0x8);                // cs
    pushToStack(process, startWrapper);      // rip

    for (int i = 0; i < 15; i++)
    { // push 15 registries
        if (i == RDI)
        {
            pushToStack(process, entryPoint);
        }
        else if (i == RSI)
        {
            pushToStack(process, argc);
        }
        else if (i == RDX)
        {
            pushToStack(process, argvAux);
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

Pipe openProcessPipe(char *name, int fds[2]){
    Process process = getCurrentProcess();
    Pipe pipe = openPipe(name);
    if(pipe == NULL){
        return NULL;
    }
    if (fds == NULL){
        return pipe;
    }
    int fd = 0;
    while(process->fds[fd] != NULL){
        if (fd == process->fdLimit){
            // TODO realloc
            return NULL;
        }
        fd++;
    }
    process->fds[fd] = pipe;
    fds[0] = fd;
    fd++;
    while(process->fds[fd] != NULL){
        if (fd == process->fdLimit){
            // TODO realloc
            return NULL;
        }
        fd++;
    }
    process->fds[fd] = pipe;
    fds[1] = fd;
    return pipe;
}

int closeProcessPipe(int fd){
    Process process = getCurrentProcess();
    if(fd < 0 || fd >= process->fdLimit){
        return -1;
    }
    if(process->fds[fd] == NULL){
        return -1;
    }
    closePipe(process->fds[fd]);
    process->fds[fd] = NULL;
    return 0;
}

int readProcessPipe(int fd, char *buffer, int size){
    Process process = getCurrentProcess();
    if(fd < 0 || fd >= process->fdLimit){
        return -1;
    }
    if(process->fds[fd] == NULL || process->pipeTypes[fd] != READ){
        return -1;
    }
    return readFromPipe(process->fds[fd], buffer, size);
}

int writeProcessPipe(int fd, char *buffer, int size){
    Process process = getCurrentProcess();
    if(fd < 0 || fd >= process->fdLimit){
        return -1;
    }
    if(process->fds[fd] == NULL || process->pipeTypes[fd] != WRITE){
        return -1;
    }
    if (process->fds[fd] == process->stdio){
        printStringLimited(WHITE,buffer, size);
        return size;
    }
    return writeToPipe(process->fds[fd], buffer, size);
}

void freeStack(Process process)
{
    free(process->stack);
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