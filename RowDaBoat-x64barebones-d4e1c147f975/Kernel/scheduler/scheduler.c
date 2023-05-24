#include <scheduler.h>
#include "videoDriver.h"

typedef struct SchedulerCDT
{
    LinkedList queue[MAX_PRIORITY];
    LinkedList deleted;
    Process currentProcess;
    Process empty;
    IteratorPtr it[MAX_PRIORITY];
    IteratorPtr itDeleted;
    int quantum;
    int quantumCounter;
    char skipQuantum;
} SchedulerCDT;

static Scheduler scheduler = NULL;

static Process processList[1]; //  printf("RSP: %x\n", stackPointer);
                               // La primera vez que me llama el kernel tengo que guardar su stackPointer en algun lado

Process getNextProcess();
void printProcesses();
Process getProcess(pid_t pid);

void testList();

static char ready = 0;



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
    char *argv[2] = {"Kernel", NULL};
    // int kernelPID = createProcess("Kernel", NULL, 1, 0, argv);
    scheduler->currentProcess = createProcess("Kernel", NULL, 1, 0, argv, &startWrapper);
    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
    argv[0] = "Empty";
    scheduler->empty = createProcess("Empty", &emptyProcess, 1, 0, argv, &startWrapper);
    insert(scheduler->queue[scheduler->empty->priority], scheduler->empty);
    // scheduler->currentProcess = getProcess(kernelPID);
    scheduler->quantum = BURST_TIME;
    scheduler->quantumCounter = BURST_TIME - 1;
    ready = 1;
}

void *schedule(void *rsp)
{
    if (scheduler != NULL && ready)
    {
        scheduler->quantumCounter++;
        if (scheduler->quantumCounter >= scheduler->quantum || scheduler->skipQuantum)
        {
            // if process skipped quantum, its priority is raised
            if (scheduler->currentProcess->state != ZOMBIE)
            {
                remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                if (scheduler->currentProcess->priority > 0 && scheduler->quantumCounter >= scheduler->quantum)
                {
                    scheduler->currentProcess->priority--;
                }
                else if (scheduler->currentProcess->priority < MAX_PRIORITY - 1 && scheduler->skipQuantum)
                {
                    scheduler->currentProcess->priority++;
                }
                insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
            }

            scheduler->skipQuantum = 0;
            scheduler->quantumCounter = 0;
            scheduler->currentProcess->stackPointer = rsp;
            if (scheduler->currentProcess->state == RUNNING)
            {
                scheduler->currentProcess->state = READY;
            }
            scheduler->currentProcess = getNextProcess();
            if (scheduler->currentProcess == NULL)
            {
                printf("No process to run\n");
                return rsp;
            }
            scheduler->currentProcess->state = RUNNING;
            return scheduler->currentProcess->stackPointer;
        }
    }
    return rsp;
}

Process getNextProcess()
{
    int currentPriority = MAX_PRIORITY - 1;
    while (currentPriority >= 0)
    {
        resetIterator(scheduler->it[currentPriority], scheduler->queue[currentPriority]);
        while (hasNext(scheduler->it[currentPriority]))
        {
            Process proc = next(scheduler->it[currentPriority]);
            if (proc->state == READY && proc->pid != EMPTY_PID)
            {
                return proc;
            }
        }
        currentPriority--;
    }
    return scheduler->empty;
}

pid_t execve(void *entryPoint, char *const argv[])
{
    Process process = createProcess(argv[0], entryPoint, MAX_PRIORITY, strToNum(argv[1], 1), argv, &startWrapper);
    insert(scheduler->queue[process->priority], process);
    return process->pid;
}

void yield()
{
    scheduler->quantumCounter = scheduler->quantum;
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
        IteratorPtr it = iterator(scheduler->queue[currentPriority]);
        while (hasNext(it))
        {
            Process proc = next(it);
            if (proc->pid == pid)
            {
                freeIterator(it);
                return proc;
            }
        }
        currentPriority--;
        freeIterator(it);
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

// void printList(LinkedList list)
// {
//     printf("printList\n");
//     Node current = list->head;
//     printf("list->head: %x\n", list->head);
//     while (current != NULL)
//     {
//         printf("Current: %x\n", current);
//         printf("Current->data: %x\n", current->data);
//         printf("Current->next: %x\n", current->next);
//         current = current->next;
//     }

//     return;
// }

// Process getProcess(pid_t pid) {
//     int currentPriority = MAX_PRIORITY - 1;
//     while(currentPriority >= 0){
//         resetIterator(scheduler->it[currentPriority], scheduler->queue[currentPriority]);
//         while(hasNext(scheduler->it[currentPriority])){
//             Process proc = next(scheduler->it[currentPriority]);
//             if (proc->pid == pid){
//                 return proc;
//             }
//         }
//         currentPriority--;
//     }
//     return NULL;
// }

// Tenemos que usar una carpeta tipo /proc, o alcanza con esto?
void printProcesses()
{
    int currentPriority = MAX_PRIORITY - 1;
    printf("  Nombre    PID  Prioridad  Foreground  Stack Pointer  Base Pointer  State\n");
    while (currentPriority >= 0)
    {
        resetIterator(scheduler->it[currentPriority], scheduler->queue[currentPriority]);
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
                printf("%d       %d          %d         0x%x       0x%x", proc->pid, proc->priority, proc->foreground, proc->stackPointer, proc->stackBase);
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
    resetIterator(scheduler->itDeleted, scheduler->deleted);
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
            printf("%d       %d          %d         0x%x       0x%x", proc->pid, proc->priority, proc->foreground, proc->stackPointer, proc->stackBase);
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
    Process process = getProcess(pid);
    insert(scheduler->deleted, process);
    remove(scheduler->queue[process->priority], process);
    free(process->stack);
    process->state = ZOMBIE;
    yield();
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
    killProcess(scheduler->currentProcess->pid);
}
