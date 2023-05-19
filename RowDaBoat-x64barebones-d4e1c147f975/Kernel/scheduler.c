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

static pid_t currentPID = 0;  // Static variable to track the current PID

static Process processList[1]; 
static int ready = 0;

void *schedule(void* rsp) {
  //  printf("RSP: %x\n",1, rsp);
    if (ready){
        // printf("Scheduling process with pid: %d\n",1, processList[0]->pid);
        // printf("Process name: %s\n",1, processList[0]->name);
        // printf("Process rsp: %x\n",1, processList[0]->rsp);
        ready = 0;
        return processList[0]->rsp;
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

    processList[0] = process;
    ready = 1;
    
   // printf("Process created with pid: %d\n",1, process->pid);

    return process->pid;
}

