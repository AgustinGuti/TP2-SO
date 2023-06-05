// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <scheduler.h>
#include "videoDriver.h"

#define SHELL_PID 0

#define EOF -1

//  Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max)
{
    uint32_t u = GetUint();
    return (u + 1.0) * 2.328306435454494e-10 * max;
}

typedef struct SchedulerCDT
{
    LinkedList queue[MAX_PRIORITY];
    Iterator it[MAX_PRIORITY];
    uint32_t ticketsIndex;
    int *priorityTickets;
    int priorityTicketsSize;
    LinkedList deleted;
    Iterator itDeleted;
    Process currentProcess;
    Process empty;
    pid_t skipPID;
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
    char autoPriority;
} SchedulerCDT;

static Scheduler scheduler = NULL;

Process getNextProcess();
Process getProcess(pid_t pid);
void *changeProcess(void *rsp);
void updateMostWaitingProcess();
void cleanChildDeletedProcesses(Process process);
void deleteProcessesFromList(LinkedList list);
void blockProcessFromProcess(Process process);
void unblockProcessFromProcess(Process process);
void killKernel();

static char ready = 0;

int pow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp /= 2;
        base *= base;
    }

    return result;
}

void shuffle(int *array, int n)
{
    if (n > 1)
    {
        int i;
        for (i = 0; i < n - 1; i++)
        {
            int j = i + GetUniform(n - i);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

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
    char *argv[1] = {NULL};

    scheduler->currentProcess = createProcess("Kernel", NULL, 1, 0, argv, &startWrapper, getpid(), NULL, 0);
    if (scheduler->currentProcess == NULL)
    {
        return;
    }
    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
    scheduler->empty = createProcess("Empty", &emptyProcess, 1, 0, argv, &startWrapper, getpid(), NULL, 0);
    scheduler->quantum = BURST_TIME;
    scheduler->quantumCounter = BURST_TIME - 1;
    scheduler->skipPID = KERNEL_PID;
    scheduler->skipQuantum = 0;
    scheduler->sleepingProcesses = createLinkedList();
    scheduler->itSleepingProcesses = iterator(scheduler->sleepingProcesses);
    scheduler->prevMillis = getMillis();
    scheduler->mostWaitingProcessPID = EMPTY_PID;
    scheduler->mostWaitingProcessTime = 0;
    scheduler->priorityTicketsSize = pow(2, MAX_PRIORITY) - 1;
    scheduler->priorityTickets = (int *)malloc(sizeof(int) * scheduler->priorityTicketsSize);
    if (scheduler->priorityTickets == NULL)
    {
        printf("Not enough memory to allocate scheduler\n");
        /*not enough memory to allocate scheduler*/
        return;
    }
    scheduler->ticketsIndex = 0;
    scheduler->autoPriority = 1;

    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        int limit = pow(2, i);
        for (int j = 0; j < limit; j++)
        {
            scheduler->priorityTickets[j + limit - 1] = i;
        }
    }

    shuffle(scheduler->priorityTickets, scheduler->priorityTicketsSize);

    ready = 1;
}

void closeScheduler()
{
    ready = 0;
    killKernel();

    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        freeIterator(scheduler->it[i]);
        deleteProcessesFromList(scheduler->queue[i]);
        destroyLinkedList(scheduler->queue[i]);
    }
    freeIterator(scheduler->itDeleted);
    deleteProcessesFromList(scheduler->deleted);
    destroyLinkedList(scheduler->deleted);
    freeIterator(scheduler->itProcessesToFree);
    deleteProcessesFromList(scheduler->processesToFree);
    destroyLinkedList(scheduler->processesToFree);
    freeIterator(scheduler->itSleepingProcesses);
    destroyLinkedList(scheduler->sleepingProcesses);
    deleteProcess(scheduler->empty);
    restartProcessPID();
    free(scheduler->priorityTickets);
    free(scheduler);
    scheduler = NULL;
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
                Process process = (Process)next(scheduler->itSleepingProcesses);
                if (process->state == SLEEPING)
                {
                    process->sleepTime -= millis - scheduler->prevMillis;
                    if (process->sleepTime <= 0)
                    {
                        remove(scheduler->sleepingProcesses, process);
                        process->state = READY;
                    }
                }
            }
            scheduler->prevMillis = millis;
            // if process skipped quantum, its priority is raised
            if (scheduler->currentProcess->state != ZOMBIE && scheduler->currentProcess->pid != EMPTY_PID)
            {
                if (scheduler->autoPriority)
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
                else
                {
                    moveToBack(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }
            }
            uint32_t ticks = ticks_elapsed();
            if (scheduler->autoPriority)
            {
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
                        updateMostWaitingProcess();
                    }
                }
            }

            void *newRsp = changeProcess(rsp);

            if (scheduler->autoPriority)
            {
                scheduler->currentProcess->waitingTime = ticks;
                if (scheduler->currentProcess->pid == scheduler->mostWaitingProcessPID)
                {
                    updateMostWaitingProcess();
                }
            }
            return newRsp;
        }
    }
    return rsp;
}

void setAutoPrio(char autoPrio)
{
    scheduler->autoPriority = autoPrio;
}

void updateMostWaitingProcess()
{
    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        /* add 1 to waitingtime for each process*/
        resetIterator(scheduler->it[i]);
        while (hasNext(scheduler->it[i]))
        {
            Process proc = next(scheduler->it[i]);
            if ((proc->waitingTime < scheduler->mostWaitingProcessTime && proc->pid >= 0) || (scheduler->mostWaitingProcessPID == EMPTY_PID && proc->pid != KERNEL_PID))
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
        LinkedList processesToFree = createLinkedList();
        resetIterator(scheduler->itProcessesToFree);
        while (hasNext(scheduler->itProcessesToFree))
        {
            Process proc = next(scheduler->itProcessesToFree);
            if (proc->pid != scheduler->currentProcess->pid)
            {
                insert(processesToFree, proc);
            }
        }
        Iterator it = iterator(processesToFree);
        while (hasNext(it))
        {
            Process proc = next(it);
            remove(scheduler->processesToFree, proc);
            freeStack(proc);
        }
        freeIterator(it);
        destroyLinkedList(processesToFree);
    }
    scheduler->currentProcess->state = RUNNING;
    return scheduler->currentProcess->stackPointer;
}

Process getNextProcess()
{
    int currentPriority = MAX_PRIORITY - 1;
    if (!scheduler->autoPriority)
    {
        currentPriority = scheduler->priorityTickets[scheduler->ticketsIndex++];
        if (scheduler->ticketsIndex == scheduler->priorityTicketsSize)
        {
            scheduler->ticketsIndex = 0;
        }
    }
    int prioritiesVisited = 0;
    while (prioritiesVisited < MAX_PRIORITY)
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
        if (currentPriority < 0)
        {
            currentPriority = MAX_PRIORITY - 1;
        }
        prioritiesVisited++;
    }
    return scheduler->empty;
}

pid_t execve(void *entryPoint, Pipe *pipes, char pipeQty, char *argv[])
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
    if (process == NULL)
    {
        return -1;
    }
    if (priority != process->priority)
    {
        insert(scheduler->queue[priority], process);
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
                case SLEEPING:
                    printf("    SLEEPING\n");
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
                if (proc->state == ZOMBIE)
                {
                    printf("    KILLED\n");
                }
            }
        }
    }
}

pid_t blockHandler(pid_t pid)
{
    Process process = getProcess(pid);
    if (process == NULL || process->pid == KERNEL_PID || process->pid == EMPTY_PID)
    {
        return -1;
    }
    if (process->state == READY || process->state == RUNNING || process->state == SLEEPING)
    {
        blockProcessFromProcess(process);
        return process->pid;
    }
    else if (process->state == BLOCKED)
    {
        unblockProcessFromProcess(process);
        return process->pid;
    }
    return -1;
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
    if (process->state == BLOCKED)
    {
        if (process->sleepTime > 0)
        {
            process->state = SLEEPING;
        }
        else
        {
            process->state = READY;
        }
        if (scheduler->currentProcess->pid == EMPTY_PID)
        {
            yield();
        }
    }
}

void killKernel()
{
    Process process = getProcess(KERNEL_PID);
    if (process != NULL)
    {
        process->foreground = 0;
        process->state = ZOMBIE;
        char newChar[1] = {EOF};
        writeProcessPipe(STDOUT, newChar, 1);
        semClose(process->waitingSem);
        remove(scheduler->queue[process->priority], process);
        cleanChildDeletedProcesses(process);

        deleteProcess(process);
    }
}

pid_t killProcess(pid_t pid)
{
    if (pid == KERNEL_PID || pid == EMPTY_PID)
    {
        printerr("No es posible matar el kernel.\n");
        return -1;
    }
    if (pid == SHELL_PID && scheduler->currentProcess->pid != SHELL_PID)
    {
        printerr("No es posible matar la shell desde otro proceso.\n");
        return -1;
    }
    Process process = getProcess(pid);
    if (process != NULL)
    {
        Process parent = getProcess(process->parentPID);
        if (parent == NULL)
        {
            printerr("No es posible matar el proceso %d, ya que su padre no existe.\n", pid);
            return -1;
        }
        process->foreground = 0;
        if (process->state == BLOCKED)
        {
            remove(scheduler->sleepingProcesses, process);
        }
        process->state = ZOMBIE;
        char newChar[1] = {EOF};
        writeProcessPipe(STDOUT, newChar, 1);
        semClose(process->waitingSem);
        remove(scheduler->queue[process->priority], process);
        cleanChildDeletedProcesses(process);

        if (parent->waitingForPID == pid)
        {
            parent->waitingForPID = -1;
            semPost(parent->waitingSem);
        }

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
                freeStack(process);
            }
        }
        return pid;
    }
    printerr("Process %d not found\n", pid);
    return -1;
}

void cleanChildDeletedProcesses(Process process)
{
    LinkedList processesToDelete = createLinkedList();

    resetIterator(scheduler->itDeleted);
    while (hasNext(scheduler->itDeleted))
    {
        Process proc = next(scheduler->itDeleted);
        if (proc->parentPID == process->pid)
        {
            insert(processesToDelete, proc);
        }
    }

    Iterator it = iterator(processesToDelete);
    while (hasNext(it))
    {
        Process proc = next(it);
        remove(scheduler->deleted, proc);
        remove(scheduler->processesToFree, proc);
        deleteProcess(proc);
    }
    freeIterator(it);
    destroyLinkedList(processesToDelete);
}

void deleteProcessesFromList(LinkedList list)
{
    Iterator it = iterator(list);
    while (hasNext(it))
    {
        Process proc = next(it);
        deleteProcess(proc);
    }
    freeIterator(it);
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
    scheduler->currentProcess->exitValue = ret;
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
    scheduler->currentProcess->state = SLEEPING;
    yield();
}