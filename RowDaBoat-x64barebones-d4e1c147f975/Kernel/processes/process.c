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



static pid_t currentPID = KERNEL_PID; // Static variable to track the current PID


Process initProcess(char *name, uint8_t priority, uint8_t foreground){
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
    // Process stack is the top of the stack, stack base is process->stack + STACK_SIZE
    process->stack = (uint64_t *)malloc(STACK_SIZE*sizeof(uint64_t));
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
    for(int i = 2; i < MAX_FDS; i++){
        process->fds[i] = -1;
    }
    return process;
}

Process createProcess(char *name, void *entryPoint, uint8_t priority, uint8_t foreground, char *argv[], void *startWrapper)
{
    Process process = initProcess(name, priority, foreground);
    int argc = 0;
    if (argv != NULL)
    {
        while (argv[argc] != NULL)
        {
            argc++;
        }
    }
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
            pushToStack(process, argv);
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

Process dupProcess(Process parentProcess)
{
    Process process = initProcess(parentProcess->name, parentProcess->priority+1, parentProcess->foreground);

    for (int i = 0; i < 15; i++){
        printf("Parent stack %d: %x\n", i, parentProcess->stack[STACK_SIZE-i-1]);
    }

    memcpy(process->stack, parentProcess->stack, STACK_SIZE * sizeof(uint64_t));

    uint64_t parentStackOffset = (uint64_t) (parentProcess->stackPointer - parentProcess->stackBase);
    uint64_t parentStackOffsetFromStack1 = (uint64_t)((uint64_t *)parentProcess->stack[STACK_SIZE - 2] - parentProcess->stackPointer);
    uint64_t parentStackOffsetFromStack2 = (uint64_t)((uint64_t *)parentProcess->stack[STACK_SIZE - 9] - parentProcess->stackPointer);
    uint64_t parentStackOffsetFromStack3 = (uint64_t)((uint64_t *)parentProcess->stack[STACK_SIZE - 13] - parentProcess->stackPointer);


    process->stackPointer = process->stackBase - parentStackOffset;
    process->stack[STACK_SIZE-2] = process->stackPointer + parentStackOffsetFromStack1;
    process->stack[STACK_SIZE-9] = process->stackPointer + parentStackOffsetFromStack2;
    process->stack[STACK_SIZE-13] = process->stackPointer + parentStackOffsetFromStack3;

    for (int i = 0; i < 15; i++){
        printf("Child stack %d: %x\n", i, process->stack[STACK_SIZE-i-1]);
    }
    process->state = READY;
    return process;
}

void emptyProcess()
{
    while (1)
    {
        _hlt();
    }
}