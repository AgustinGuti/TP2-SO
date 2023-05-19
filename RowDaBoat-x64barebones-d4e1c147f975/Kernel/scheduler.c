#include "scheduler.h"
#include "videoDriver.h"

typedef struct ProcessCDT{
    pid_t pid;
    char *name;
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
    Process it;
}SchedulerCDT;

static Scheduler scheduler = NULL;
static pid_t currentPID = 0;  // Static variable to track the current PID

static Process processList[1];   //  printf("RSP: %x\n",1, stackPointer);
  //La primera vez que me llama el kernel tengo que guardar su stackPointer en algun lado
static int currentProcess = 0;
static int processCount = 0;
static int ready = 0;

Process getNextProcess() ;

void initScheduler() {
    scheduler = (Scheduler) malloc(sizeof(SchedulerCDT));
    if( scheduler == NULL){
        /*not enough memory to allocate scheduler*/
        return;
    }
    printf("Scheduler initialized\n",0);
    scheduler->processList = createLinkedList();
    int kernelPID = createProcess("Kernel", NULL, 0, 1, NULL);
    scheduler->it = iterator(scheduler->processList);
    scheduler->currentProcess = NULL;
}

void * schedule(void* rsp) {
    if (scheduler != NULL){
        if (scheduler->currentProcess != NULL){
            scheduler->currentProcess->stackPointer = rsp;
            if (scheduler->currentProcess->state == RUNNING){
                scheduler->currentProcess->state = READY;
            }
        }
        scheduler->currentProcess = getNextProcess();
        if (scheduler->currentProcess != NULL){
            scheduler->currentProcess->state = RUNNING;
            return scheduler->currentProcess->stackPointer;
        }              
    }
    return rsp; //TODO noop
}

Process getNextProcess() {
    scheduler->it = next(scheduler->processList);
    int count = 0;
    while (((Process)getData(scheduler->it))->state != READY && count < getSize(scheduler->processList)){
        scheduler->it = next(scheduler->processList);
        count++;
    }
    if (count == getSize(scheduler->processList)){
        return NULL;    ///No op
    }
    return ((Process)getData(scheduler->it));
}

int fork() {
    return -1;
}

int execve(void* entryPoint, char * const argv[]){
    return -1; //createProcess(argv[0], entryPoint, 1, 1, argv);
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

pid_t createProcess(char* name, void* entryPoint, uint8_t priority, uint8_t foreground, char * argv[]) {

    /*if( entryPoint == NULL ) {
        return -1;
    }*/
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
    pushToStack(process, entryPoint);              //rip

    for(int i = 0; i < 15; i++){     //push 15 registries
        if (i == 5){    //rdi
            pushToStack(process, argc);
        }else if (i == 6){  //rsi
            pushToStack(process, argv);
        }else{
            pushToStack(process, 0x0);   
        }
    }
    
    insert(scheduler->processList, process);
    if (entryPoint == NULL){
        process->state = BLOCKED;
    }else{
        process->state = READY;
    }

    return process->pid;
}

