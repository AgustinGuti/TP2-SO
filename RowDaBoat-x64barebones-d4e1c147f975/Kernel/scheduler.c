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
}SchedulerCDT;

static Scheduler scheduler;
static pid_t currentPID = 0;  // Static variable to track the current PID

static Process processList[1];   //  printf("RSP: %x\n",1, stackPointer);
  //La primera vez que me llama el kernel tengo que guardar su stackPointer en algun lado
static int currentProcess = 0;
static int processCount = 0;
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
        //No hacerlo si viene del kernel ?
        //processList[currentProcess]->stackPointer = stackPointer;

      /*  currentProcess = (currentProcess + 1) % 3;
        if (currentProcess >= processCount) {
            currentProcess = 0;
        }
        currentProcess = 0;*/
        // printf("Scheduling process with pid: %d\n",1, processList[0]->pid);
        // printf("Process name: %s\n",1, processList[0]->name);
        // printf("Process stackPointer: %x\n",1, processList[0]->stackPointer);
        ready = 0;
        return scheduler->currentProcess->stackPointer;
    }
    return stackPointer;
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

    process->rsp = process->stackPointer;
    insert(scheduler->processList, process);
    scheduler->currentProcess = process;
    ready = 1;
    return process->pid;
}

