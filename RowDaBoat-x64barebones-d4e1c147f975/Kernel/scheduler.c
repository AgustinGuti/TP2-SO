#include "scheduler.h"
#include "videoDriver.h"

typedef struct ProcessCDT{
    pid_t pid;
    char *name;
    void* rsp;
    processState state;
    uint8_t priority;
    uint64_t *stack;
    uint64_t *stackBase;
    uint64_t *stackPointer;
    uint8_t foreground;
}ProcessCDT;

typedef struct SchedulerCDT{
    LinkedList processList;
    Process currentProcess;
}SchedulerCDT;

static Scheduler scheduler;
static pid_t currentPID = 0;  // Static variable to track the current PID

static int ready = 0;

void initScheduler() {
    scheduler = (Scheduler) malloc(sizeof(SchedulerCDT));
    if( scheduler == NULL){
        /*not enough memory to allocate scheduler*/
        return;
    }
    printf("Scheduler initialized\n",1);
    scheduler->processList = createLinkedList();
    scheduler->currentProcess = NULL;
}

void * schedule(void* rsp) {
    if (ready){
        ready = 0;
        return scheduler->currentProcess->rsp;
    }
    return rsp;
}

int fork() {
    return -1;
}

int execve(void* entryPoint, char * const argv[]){
    return -1;
}

pid_t generatePID() {
    return currentPID++;         // Increment and return the new PID
}

void pushStack(Process process, uint64_t value) {
   *(--process->stackPointer) = value;
}


pid_t createProcess(char* name, void* entryPoint, uint8_t priority, uint8_t foreground, char * argv[]) {

    if( entryPoint == NULL ) {
        return -1;
    }
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
    pushStack(process, 0x0);                     //ss
    pushStack(process, process->stackBase);      //rsp
    pushStack(process, 0x202);                   //rflags
    pushStack(process, 0x8);                     //cs
    pushStack(process, entryPoint);              //rip

    //Si hay parametros van en algun lado aca adentro
    for(int i = 0; i < 15; i++){
        if (i == 5){
            pushStack(process, argc);
        }else if (i == 6){
            pushStack(process, argv);
        }else{
            pushStack(process, 0x0);    //push 15 registries
        }
    }

    process->rsp = process->stackPointer;
    insert(scheduler->processList, process);
    scheduler->currentProcess = process;
    ready = 1;
    return process->pid;
}

