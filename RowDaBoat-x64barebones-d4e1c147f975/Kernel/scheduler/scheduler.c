#include <scheduler.h>
#include "videoDriver.h"

#define SHELL_PID 0

#define EOF -1

typedef struct SchedulerCDT
{
    LinkedList queue[MAX_PRIORITY];
    LinkedList deleted;
    Process currentProcess;
    Process empty;
    Iterator it[MAX_PRIORITY];
    Iterator itDeleted;
    char skipPID;
    int quantum;
    int quantumCounter;
    char skipQuantum;
    LinkedList processesToFree;
    Iterator itProcessesToFree;
    LinkedList sleepingProcesses;
    Iterator itSleepingProcesses;
    uint64_t prevMillis;
    pid_t mostWaitingProcessPID;
    uint64_t mostWaitingProcessTime;
} SchedulerCDT;

static Scheduler scheduler = NULL;

Process getNextProcess();
Process getProcess(pid_t pid);
void *changeProcess(void *rsp);
void updateMostWaitingProcess();

static char ready = 0;
static uint64_t counter = 0;

void initScheduler()
{
    scheduler = (Scheduler)malloc(sizeof(SchedulerCDT));
    if (scheduler == NULL)
    {
        printf("Not enough memory to allocate scheduler\n");
        /*not enough memory to allocate scheduler*/
        return;
    }
    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        scheduler->queue[i] = createLinkedList();
        scheduler->it[i] = iterator(scheduler->queue[i]);
    }
    scheduler->deleted = createLinkedList();
    scheduler->itDeleted = iterator(scheduler->deleted);
    scheduler->processesToFree = createLinkedList();
    scheduler->itProcessesToFree = iterator(scheduler->processesToFree);
    char *argv[2] = {"Kernel", NULL};
    // int kernelPID = createProcess("Kernel", NULL, 1, 0, argv);

    scheduler->currentProcess = createProcess("Kernel", NULL, 1, 0, argv, &startWrapper, getpid(), NULL, 0);
    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
    argv[0] = "Empty";
    scheduler->empty = createProcess("Empty", &emptyProcess, 1, 0, argv, &startWrapper, getpid(), NULL, 0);
    // scheduler->currentProcess = getProcess(kernelPID);
    scheduler->quantum = BURST_TIME;
    scheduler->quantumCounter = BURST_TIME - 1;
    scheduler->skipPID = KERNEL_PID;
    scheduler->skipQuantum = 0;
    scheduler->sleepingProcesses = createLinkedList();
    scheduler->itSleepingProcesses = iterator(scheduler->sleepingProcesses);
    scheduler->prevMillis = getMillis();
    scheduler->mostWaitingProcessPID = EMPTY_PID;
    scheduler->mostWaitingProcessTime = 0;
    ready = 1;
}

void *schedule(void *rsp)
{
    if (scheduler != NULL && ready)
    {
        scheduler->quantumCounter++;
        // enter if scheduler needs to change process
        if (scheduler->quantumCounter >= scheduler->quantum || scheduler->skipQuantum || scheduler->skipPID == scheduler->currentProcess->pid)
        {
            resetIterator(scheduler->itSleepingProcesses);
            uint64_t millis = getMillis();
            while (hasNext(scheduler->itSleepingProcesses))
            {
                Process p = (Process)next(scheduler->itSleepingProcesses);
                p->sleepTime -= millis - scheduler->prevMillis;
                if (p->sleepTime <= 0)
                {
                    remove(scheduler->sleepingProcesses, p);
                    unblockProcessFromProcess(p);
                }
            }
            scheduler->prevMillis = millis;
            // if process skipped quantum, its priority is raised
            if (scheduler->currentProcess->state != ZOMBIE && scheduler->currentProcess->pid != EMPTY_PID)
            {
                if (scheduler->skipPID == scheduler->currentProcess->pid)
                {
                    scheduler->skipPID = KERNEL_PID;
                    moveToBack(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }
                else if (scheduler->currentProcess->priority > 0 && scheduler->quantumCounter >= scheduler->quantum)
                {
                    remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    scheduler->currentProcess->priority--;
                    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }
                else if (scheduler->currentProcess->priority < MAX_PRIORITY - 1 && scheduler->skipQuantum)
                {
                    remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    scheduler->currentProcess->priority++;
                    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }
                else
                {
                    moveToBack(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }
            }
            uint32_t ticks = ticks_elapsed();
            Process mostWaitingProcess = getProcess(scheduler->mostWaitingProcessPID);
            if (mostWaitingProcess != NULL && mostWaitingProcess->state != ZOMBIE)
            {
                if (ticks - scheduler->mostWaitingProcessTime > MAX_WAITING_TIME)
                {
                    mostWaitingProcess->waitingTime = ticks;
                    remove(scheduler->queue[mostWaitingProcess->priority], mostWaitingProcess);
                    mostWaitingProcess->priority = MAX_PRIORITY - 1;
                    insert(scheduler->queue[mostWaitingProcess->priority], mostWaitingProcess);
                    scheduler->mostWaitingProcessPID = EMPTY_PID;
                    scheduler->mostWaitingProcessTime = ticks;
                    /*update the waiting time for each process*/
                    updateMostWaitingProcess();
                }
            }
            void *newRsp = changeProcess(rsp);
            /* waiting time is set when process starts to run */
            scheduler->currentProcess->waitingTime = ticks;
            if (scheduler->currentProcess->pid == scheduler->mostWaitingProcessPID)
            {
                updateMostWaitingProcess();
            }
            return newRsp;
        }
    }
    return rsp;
}

void updateMostWaitingProcess()
{
    int currentPriority = scheduler->currentProcess->priority;
    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        /* add 1 to waitingtime for each process*/
        resetIterator(scheduler->it[i]);
        while (hasNext(scheduler->it[i]))
        {
            Process proc = next(scheduler->it[i]);
            if (proc->waitingTime < scheduler->mostWaitingProcessTime && proc->pid >= 0 || scheduler->mostWaitingProcessPID == EMPTY_PID && proc->pid != KERNEL_PID)
            {
                scheduler->mostWaitingProcessPID = proc->pid;
                scheduler->mostWaitingProcessTime = proc->waitingTime;
            }
        }
    }
}

void *changeProcess(void *rsp)
{
    scheduler->skipQuantum = 0;
    scheduler->quantumCounter = 0;
    scheduler->currentProcess->stackPointer = rsp;
    if (scheduler->currentProcess->state == RUNNING)
    {
        scheduler->currentProcess->state = READY;
    }
    scheduler->currentProcess = getNextProcess();
    if (getSize(scheduler->processesToFree) > 0)
    {
        resetIterator(scheduler->itProcessesToFree);
        while (hasNext(scheduler->itProcessesToFree))
        {
            Process proc = next(scheduler->itProcessesToFree);
            if (proc->pid != scheduler->currentProcess->pid)
            {
                freeStack(proc);
                remove(scheduler->processesToFree, proc);
            }
        }
    }
    scheduler->currentProcess->state = RUNNING;
    return scheduler->currentProcess->stackPointer;
}

Process getNextProcess()
{
    int currentPriority = MAX_PRIORITY - 1;
    while (currentPriority >= 0)
    {
        resetIterator(scheduler->it[currentPriority]);
        while (hasNext(scheduler->it[currentPriority]))
        {
            Process proc = next(scheduler->it[currentPriority]);
            if (proc->state == READY && proc->pid != scheduler->skipPID && proc->pid != EMPTY_PID)
            {
                return proc;
            }
        }
        currentPriority--;
    }
    return scheduler->empty;
}

pid_t execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[])
{
    int foreground = strToNum(argv[1], 1);
    Process process = createProcess(argv[0], entryPoint, MAX_PRIORITY, foreground, &argv[2], &startWrapper, getpid(), pipes, pipeQty);
    if (foreground)
    {
        int currentForeground = scheduler->currentProcess->foreground;
        scheduler->currentProcess->foreground = 0;
        insert(scheduler->queue[process->priority], process);
        waitpid(process->pid);
        scheduler->currentProcess->foreground = currentForeground;
    }
    else
    {
        insert(scheduler->queue[process->priority], process);
    }
    return process->pid;
}

void yield()
{
    // scheduler->quantumCounter = scheduler->quantum;
    scheduler->skipPID = scheduler->currentProcess->pid;
    triggerTimer();
}

void exit(int value)
{
    killProcess(scheduler->currentProcess->pid);
}

pid_t getpid()
{
    return scheduler->currentProcess->pid;
}

Process getProcess(pid_t pid)
{
    int currentPriority = MAX_PRIORITY - 1;
    while (currentPriority >= 0)
    {
        resetIterator(scheduler->it[currentPriority]);
        while (hasNext(scheduler->it[currentPriority]))
        {
            Process proc = next(scheduler->it[currentPriority]);
            if (proc->pid == pid)
            {
                return proc;
            }
        }
        currentPriority--;
    }
    return NULL;
}

int nice(pid_t pid, int priority)
{
    if (priority >= MAX_PRIORITY || priority < 0)
    {
        return -1;
    }
    Process process = getProcess(pid);
    if (priority != process->priority)
    {
        insert(scheduler->queue[priority], process);
        // printList(scheduler->queue[priority]);
        remove(scheduler->queue[process->priority], process);
        process->priority = priority;
    }
    return priority;
}

// Tenemos que usar una carpeta tipo /proc, o alcanza con esto?
void printProcesses(char showKilled)
{
    int currentPriority = MAX_PRIORITY - 1;
    printf(" Nombre    PID  ParentPID  Prioridad  Foreground  Stack Pointer  Base Pointer  State\n");
    while (currentPriority >= 0)
    {
        resetIterator(scheduler->it[currentPriority]);
        while (hasNext(scheduler->it[currentPriority]))
        {
            Process proc = next(scheduler->it[currentPriority]);
            if (proc->pid != KERNEL_PID)
            {
                int nameLenght = strlen(proc->name);
                printf(" %s  ", proc->name);
                if (nameLenght < 10)
                {
                    for (int i = 0; i < 10 - nameLenght; i++)
                    {
                        printf(" ");
                    }
                }
                printf("%d       %d       %d          %d         0x%x       0x%x", proc->pid, proc->parentPID, proc->priority, proc->foreground, proc->stackPointer, proc->stackBase);
                char state = proc->state;
                switch (state)
                {
                case READY:
                    printf("    READY\n");
                    break;
                case RUNNING:
                    printf("    RUNNING\n");
                    break;
                case BLOCKED:
                    printf("    BLOCKED\n");
                    break;
                case ZOMBIE:
                    printf("    KILLED\n");
                    break;
                }
            }
        }
        currentPriority--;
    }
    if (showKilled)
    {
        resetIterator(scheduler->itDeleted);
        while (hasNext(scheduler->itDeleted))
        {
            Process proc = next(scheduler->itDeleted);
            if (proc->pid != KERNEL_PID)
            {
                int nameLenght = strlen(proc->name);
                printf(" %s  ", proc->name);
                if (nameLenght < 10)
                {
                    for (int i = 0; i < 10 - nameLenght; i++)
                    {
                        printf(" ");
                    }
                }
                printf("%d       %d       %d          %d         0x%x       0x%x", proc->pid, proc->parentPID, proc->priority, proc->foreground, proc->stackPointer, proc->stackBase);
                char state = proc->state;
                switch (state)
                {
                case READY:
                    printf("    READY\n");
                    break;
                case RUNNING:
                    printf("    RUNNING\n");
                    break;
                case BLOCKED:
                    printf("    BLOCKED\n");
                    break;
                case ZOMBIE:
                    printf("    KILLED\n");
                    break;
                }
            }
        }
    }
}

void blockHandler(pid_t pid)
{
    Process process = getProcess(pid);
    if (process->state == READY || process->state == RUNNING)
    {
        blockProcessFromProcess(process);
    }
    else if (process->state == BLOCKED)
    {
        unblockProcessFromProcess(process);
    }
}

void blockProcess(pid_t pid)
{
    Process process = getProcess(pid);
    blockProcessFromProcess(process);
}

void blockProcessFromProcess(Process process)
{
    process->state = BLOCKED;
    if (process->pid == scheduler->currentProcess->pid)
    {
        scheduler->skipQuantum = 1;
        triggerTimer();
    }
    return;
}

void unblockProcess(pid_t pid)
{
    Process process = getProcess(pid);
    unblockProcessFromProcess(process);
}

void unblockProcessFromProcess(Process process)
{
    process->state = READY;
    if (scheduler->currentProcess->pid == EMPTY_PID)
    {
        yield();
    }
}

void killProcess(pid_t pid)
{
    if (pid == SHELL_PID && scheduler->currentProcess->pid != SHELL_PID)
    {
        printerr("No es posible matar la shell desde otro proceso.\n");
        return;
    }
    Process process = getProcess(pid);
    if (process != NULL)
    {
        Process parent = getProcess(process->parentPID);
        if (parent->waitingForPID == process->pid)
        {
            parent->waitingForPID = -1;
            semPost(parent->waitingSem);
        }
        process->foreground = 0;
        process->state = ZOMBIE;
        char newChar[1] = {EOF};
        writeProcessPipe(STDOUT, newChar, 1);
        semClose(process->waitingSem);
        remove(scheduler->queue[process->priority], process);
        closePipes(process);
        int i;
        for (i = 0; i < process->argc; i++)
        {
            free(process->argv[i]);
        }
        free(process->argv);

        if (parent->state == ZOMBIE)
        {
            deleteProcess(process);
        }
        else
        {
            insert(scheduler->deleted, process);
            if (process->pid == scheduler->currentProcess->pid)
            {
                insert(scheduler->processesToFree, process);
                yield();
            }
            else
            {
                free(process->stack);
            }
        }
    }
    else
    {
        printerr("Process %d not found\n", pid);
    }
}

Process getForegroundProcess()
{
    int currentPriority = MAX_PRIORITY - 1;
    while (currentPriority >= 0)
    {
        resetIterator(scheduler->it[currentPriority]);
        while (hasNext(scheduler->it[currentPriority]))
        {
            Process proc = next(scheduler->it[currentPriority]);
            if (proc->foreground)
            {
                return proc;
            }
        }
        currentPriority--;
    }
    return NULL;
}

void killForegroundProcess()
{
    Process process = getForegroundProcess();
    if (process != NULL)
    {
        killProcess(process->pid);
    }
}

void startWrapper(void *entryPoint, char argc, char *argv[])
{
    int ret = ((int (*)(int, char *[]))entryPoint)(argc, argv);
    if (scheduler->currentProcess->pid == 0)
    {
        Process kernel = getProcess(KERNEL_PID);
        kernel->state = READY;
        printf("Kernel ready\n");
    }
    // printf("Process %d finished with return value %d\n", scheduler->currentProcess->pid, ret);
    killProcess(scheduler->currentProcess->pid);
}

pid_t waitpid(pid_t pid)
{
    Process process = getProcess(pid);
    if (process == NULL)
    {
        return -1;
    }
    if (process->state == ZOMBIE)
    {
        return pid;
    }
    Process parent = scheduler->currentProcess;
    parent->waitingForPID = pid;
    if (parent->waitingSem == NULL)
    {
        parent->waitingSem = semOpen(NULL, 0);
    }
    semWait(parent->waitingSem);
    return pid;
}

Process getCurrentProcess()
{
    return scheduler->currentProcess;
}

void sleep(int millis)
{
    scheduler->currentProcess->sleepTime = millis + getMillis() - scheduler->prevMillis;
    insert(scheduler->sleepingProcesses, scheduler->currentProcess);
    blockProcessFromProcess(scheduler->currentProcess);
}