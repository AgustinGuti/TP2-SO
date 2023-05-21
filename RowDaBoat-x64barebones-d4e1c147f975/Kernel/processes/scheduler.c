#include "scheduler.h"
#include "videoDriver.h"

enum {RAX, RBX, RCX, RDX, RBP, RDI, RSI, R8, R9, R10, R11, R12, R13, R14, R15};

#define KERNEL_PID -1
typedef struct ProcessCDT{
    pid_t pid;
    char *name;
    processState state;
    int priority;
    uint64_t *stack;
    uint64_t *stackBase;
    uint64_t *stackPointer;
    uint8_t foreground;
}ProcessCDT;

typedef struct SchedulerCDT{
    LinkedList queue[MAX_PRIORITY];
    Process currentProcess;
    Process it;
    int quantum;
    int quantumCounter;
    char skipQuantum;
}SchedulerCDT;

static Scheduler scheduler = NULL;
static pid_t currentPID = KERNEL_PID;  // Static variable to track the current PID

static Process processList[1];   //  printf("RSP: %x\n",1, stackPointer);
  //La primera vez que me llama el kernel tengo que guardar su stackPointer en algun lado

Process getNextProcess() ;
void printProcesses();
Process getProcess(pid_t pid);


void initScheduler() {
    scheduler = (Scheduler) malloc(sizeof(SchedulerCDT));
    if( scheduler == NULL){
        /*not enough memory to allocate scheduler*/
        return;
    }
    //scheduler->processList = createLinkedList();
    for(int i = 0; i < MAX_PRIORITY; i++){
        scheduler->queue[i] = createLinkedList();
    }
    int kernelPID = createProcess("Kernel", NULL, 1, 1, NULL);
    scheduler->it = iterator(scheduler->queue[MAX_PRIORITY-1]);
    scheduler->currentProcess = getProcess(kernelPID);
    scheduler->quantum = BURST_TIME;
    scheduler->quantumCounter = BURST_TIME-1;
}


void * schedule(void* rsp) {
    if (scheduler != NULL){
        scheduler->quantumCounter++;
        if(scheduler->quantumCounter >= scheduler->quantum || scheduler->skipQuantum){
            //if process used all its quantum, its priotity is lowered
            if (scheduler->quantumCounter >= scheduler->quantum){
                if (scheduler->currentProcess->priority > 0){
                    printf("Process %d used all its quantum\n", 1, scheduler->currentProcess->pid);
                    remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    // printProcesses();
                    scheduler->currentProcess->priority--;
                    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    // printProcesses();
                }
            }else{ //if process skipped quantum, its priority is raised
                if (scheduler->currentProcess->priority < MAX_PRIORITY - 1){
                    printf("Process %d skipped quantum\n", 1, scheduler->currentProcess->pid);
                    remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    // printProcesses();
                    scheduler->currentProcess->priority++;
                    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    // printProcesses();

                }
            }
            scheduler->skipQuantum = 0;
            scheduler->quantumCounter = 0;
            scheduler->currentProcess->stackPointer = rsp;
            if (scheduler->currentProcess->state == RUNNING){
                scheduler->currentProcess->state = READY;
            }
            scheduler->currentProcess = getNextProcess();
            if (scheduler->currentProcess != NULL){
                //printf("Next process: %d\n", 1, scheduler->currentProcess->pid);
                scheduler->currentProcess->state = RUNNING;
                return scheduler->currentProcess->stackPointer;
            }
        }
    }
    return rsp;
}

Process getNextProcess() {
    int currentPriority = MAX_PRIORITY - 1;
    while(currentPriority >= 0){
        Process it = iterator(scheduler->queue[currentPriority]);
        while(it != NULL){
            if (((Process)getData(it))->state == READY){
                return ((Process)getData(it));
            }
            it = next(it);
        }
        currentPriority--;
    }
    //TODO return no op
    return NULL;
}

int fork() {
    return -1;
}

void yield() {
    scheduler->skipQuantum = 1;
    triggerTimer();
}

void exit(int value) {
    killProcess();
}

pid_t getpid() {
    return scheduler->currentProcess->pid;
}

static int first = 1;
int execve(void* entryPoint, char * const argv[]){
    if(first){
        first = 0;
        return createProcess(argv[0], entryPoint, 3, 1, argv);
    }else{

    return createProcess(argv[0], entryPoint, 2, 1, argv);
    }
}

pid_t generatePID() {
    return currentPID++;         // Increment and return the new PID
}

void pushToStack(Process process, uint64_t value) {
   *(--process->stackPointer) = value;
}

uint64_t popFromStack(Process process) {
    return *(process->stackPointer++);
}

Process getProcess(pid_t pid){
    int idx = 0;
    Process it = iterator(scheduler->queue[idx]);

    while (idx < MAX_PRIORITY){
        while(it == NULL){
            idx++;
            it = iterator(scheduler->queue[idx]);
        }
        if (idx >= MAX_PRIORITY) break;
        if (((Process)getData(it))->pid == pid){
            return ((Process)getData(it));
        }

        it = next(it);

    }
    return NULL;
}

//Tenemos que usar una carpeta tipo /proc, o alcanza con esto?
void printProcesses(){
    int currentPriority = MAX_PRIORITY - 1;
    printf("  Nombre    PID  Prioridad  Foreground  Stack Pointer  Base Pointer  State\n", 0);
    //Empezamos a imprimir desde el primer proceso que no es el kernel
    // while (((Process)getData(it))->pid > 0){ 
    //     it = next(it);
    // }
    while(currentPriority >= 0){
        Process it = iterator(scheduler->queue[currentPriority--]);
        while (it != NULL){
            if (((Process)getData(it))->pid != KERNEL_PID){
                int nameLenght = strlen(((Process)getData(it))->name);
                printf(" %s  ", 1, ((Process)getData(it))->name);
                if (nameLenght < 10){
                    for (int i = 0; i < 10 - nameLenght; i++){
                        printf(" ", 0);
                    }
                }
                printf("%d       %d          %d         0x%x       0x%x", 5, ((Process)getData(it))->pid, ((Process)getData(it))->priority, ((Process)getData(it))->foreground, ((Process)getData(it))->stackPointer, ((Process)getData(it))->stackBase);
                char state = ((Process)getData(it))->state;
                switch(state){
                    case READY:
                        printf("    READY\n", 0);
                        break;
                    case RUNNING:
                        printf("    RUNNING\n", 0);
                        break;
                    case BLOCKED:
                        printf("    BLOCKED\n", 0);
                        break;
                    case ZOMBIE:
                        printf("    KILLED\n", 0);
                        break;
                }
            }
            it = next(it);
        }
    }
}

void blockProcess(int pid) {
    if (pid == scheduler->currentProcess->pid){
        scheduler->currentProcess->state = BLOCKED;
        scheduler->quantumCounter = scheduler->quantum;
        triggerTimer();
    }
    Process process = getProcess(pid);
    process->state = BLOCKED;
}

void unblockProcess(int pid) {
    Process process = getProcess(pid);
    process->state = READY;
}

void killProcess() {
    scheduler->currentProcess->state = ZOMBIE;
    if (scheduler->currentProcess->pid == 0){
        Process kernel = getProcess(-1);
        kernel->state = READY;
    }
    scheduler->quantumCounter = scheduler->quantum;
    triggerTimer();
}

void startWrapper(void* entryPoint, char argc, char * argv[]) {
    int ret = ((int (*)(int, char *[]))entryPoint)(argc, argv);
    killProcess();
}


pid_t createProcess(char* name, void* entryPoint, uint8_t priority, uint8_t foreground, char * argv[]) {

    Process process = (Process) malloc(sizeof(ProcessCDT));
    if(process == NULL){
        /*not enough memory to allocate process*/
        return NULL;
    }
    process->pid = generatePID();
    process->name = (char*) malloc(strlen(name)+1);
    if(process->name == NULL ){
        /*not enough memory for process->name*/
        return NULL;
    }
    strcpy(process->name, name);
    process->priority = priority;
    process->foreground = foreground;
    int argc = 0;
    while(argv[argc] != NULL){
        argc++;
    }
    
    //Process stack is the top of the stack, stack base is process->stack + STACK_SIZE
    process->stack = (uint64_t*) malloc(STACK_SIZE);
    if(process->stack == NULL){
        /*not enough memory for process->stack*/
        return NULL;
    }
    process->stackBase = process->stack + STACK_SIZE;
    process->stackPointer = process->stackBase;
    pushToStack(process, 0x0);                     //ss
    pushToStack(process, process->stackBase);      //stackPointer
    pushToStack(process, 0x202);                   //rflags
    pushToStack(process, 0x8);                     //cs
    pushToStack(process, &startWrapper);              //rip

    for(int i = 0; i < 15; i++){     //push 15 registries
        if (i == RDI){    
            pushToStack(process, entryPoint);
        }else if (i == RSI){ 
            pushToStack(process, argc);
        }else if (i == RDX){ 
            pushToStack(process, argv);
        }else{
            pushToStack(process, 0x0);   
        }
    }
    
    insert(scheduler->queue[process->priority-1], process);
    if (entryPoint == NULL){
        process->state = BLOCKED;
    }else{
        process->state = READY;
    }

    return process->pid;
}

