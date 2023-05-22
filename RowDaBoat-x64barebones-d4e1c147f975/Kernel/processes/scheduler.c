#include <scheduler.h>
#include "videoDriver.h"
#include "linkedList.h"

enum {RAX, RBX, RCX, RDX, RBP, RDI, RSI, R8, R9, R10, R11, R12, R13, R14, R15};

#define KERNEL_PID -2
#define EMPTY_PID -1
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
    LinkedList deleted;
    Process currentProcess;
    Process empty;
    int quantum;
    int quantumCounter;
    char skipQuantum;
}SchedulerCDT;

static Scheduler scheduler = NULL;
static pid_t currentPID = KERNEL_PID;  // Static variable to track the current PID

static Process processList[1];   //  printf("RSP: %x\n", stackPointer);
  //La primera vez que me llama el kernel tengo que guardar su stackPointer en algun lado

Process getNextProcess() ;
void printProcesses();
Process getProcess(pid_t pid);
void emptyProcess();

void testList();

static char ready = 0;

void initScheduler() {
   // printf("Initializing scheduler\n");
    scheduler = (Scheduler) malloc(sizeof(SchedulerCDT));
   // printf("Scheduler: %x\n", scheduler);
    if( scheduler == NULL){
        printf("Not enough memory to allocate scheduler\n");
        /*not enough memory to allocate scheduler*/
        return;
    }
   // printf("Scheduler created\n");
    for(int i = 0; i < MAX_PRIORITY; i++){
        printf("Creating queue %d\n", i);
        scheduler->queue[i] = createLinkedList();
    }
   // testList();
    printf("Queues created\n");
   // scheduler->deleted = createLinkedList();
    int kernelPID = createProcess("Kernel", NULL, 1, 0, NULL);
    printf("Kernel PID: %d\n", kernelPID);
    int emptyPID = createProcess("Empty", &emptyProcess, 1, 0, NULL);
    printf("Empty PID: %d\n", emptyPID);
    scheduler->currentProcess = getProcess(kernelPID);
    printf("Current process: %x\n", scheduler->currentProcess);
    scheduler->quantum = BURST_TIME;
    scheduler->quantumCounter = BURST_TIME-1;
    ready = 1;
    printf("Scheduler initialized\n");
}


void * schedule(void* rsp) {
  //  printf("Scheduling\n");
  //  printf("RSP: %x\n", rsp);
    if (scheduler != NULL && ready){
        scheduler->quantumCounter++;
        if(scheduler->quantumCounter >= scheduler->quantum || scheduler->skipQuantum){
            //if process used all its quantum, its priotity is lowered
            //printf("Quantum counter: %d\n", scheduler->quantumCounter);
            if (scheduler->quantumCounter >= scheduler->quantum){
                if (scheduler->currentProcess->priority > 0){
                    remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    scheduler->currentProcess->priority--;
                    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }else{
                    headToBack(scheduler->queue[scheduler->currentProcess->priority]);
                }
            }else{ //if process skipped quantum, its priority is raised
                if (scheduler->currentProcess->priority < MAX_PRIORITY - 1){
                    remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                    scheduler->currentProcess->priority++;
                    insert(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
                }else{
                    headToBack(scheduler->queue[scheduler->currentProcess->priority]);
                }
            }
            scheduler->skipQuantum = 0;
            scheduler->quantumCounter = 0;
            scheduler->currentProcess->stackPointer = rsp;
            if (scheduler->currentProcess->state == RUNNING){
                scheduler->currentProcess->state = READY;
            }
            // printProcesses();
           // printf("Current process: %d\n", scheduler->currentProcess->pid);
            scheduler->currentProcess = getNextProcess();
            if (scheduler->currentProcess == NULL){
                return rsp;
            }
           // printf("Next process: %d\n", scheduler->currentProcess->pid);
            scheduler->currentProcess->state = RUNNING;
            return scheduler->currentProcess->stackPointer;
        }
    }
 //   printf("No process to run\n");
    return rsp;
}

Process getNextProcess() {
    int currentPriority = MAX_PRIORITY - 1;
  //  printf("Getting next process\n");
    while(currentPriority >= 0){
        IteratorPtr it = iterator(scheduler->queue[currentPriority]);
      //  printf("Current priority: %d\n", currentPriority);
        while(hasNext(it)){
            Process proc = next(it);
            if (proc->state == READY){
                freeIterator(it);
                return proc;
            }
        }
        currentPriority--;
        freeIterator(it);
    }
   // printf("No process to run\n");
    return NULL;
}

void emptyProcess(){
    while (1){
        _hlt();
    }
}

int fork() {
    return -1;
}

void yield() {
    //scheduler->quantumCounter = BURST_TIME;
     scheduler->skipQuantum = 1;
   // triggerTimer();
}

void exit(int value) {
    killProcess();
}

pid_t getpid() {
  //  printf("PID: %d\n", scheduler->currentProcess->pid);
    return scheduler->currentProcess->pid;
}

int execve(void* entryPoint, char * const argv[]){
    return createProcess(argv[0], entryPoint, MAX_PRIORITY, strToNum(argv[1], 1), argv);
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
    int currentPriority = MAX_PRIORITY - 1;
    //printf("Getting process %d\n", pid);
    while (currentPriority >= 0){
        //printf("Current priority: %d\n", currentPriority);
        IteratorPtr it = iterator(scheduler->queue[currentPriority]);
        //printf("Iterator created %x\n", it);
        //printf("Has next: %d\n", hasNext(it));
        while(hasNext(it)){
            //printf("Has next 2\n");
            Process proc = next(it);
            //printf("Process: %s\n", proc->name);
            if (proc->pid == pid){
                //printf("Process found %s\n", proc->name);
                freeIterator(it);
                //printf("Process found 2%s\n", proc->name);
                return proc;
            }
        }
        currentPriority--;
        freeIterator(it);
    }
    //printf("Process not found\n");
    return NULL;
}

//Tenemos que usar una carpeta tipo /proc, o alcanza con esto?
void printProcesses(){
    int currentPriority = MAX_PRIORITY - 1;
    printf("  Nombre    PID  Prioridad  Foreground  Stack Pointer  Base Pointer  State\n");
    while(currentPriority >= 0){
       // printf("Priority: %d\n", currentPriority);
        IteratorPtr it = iterator(scheduler->queue[currentPriority--]);
        Process proc = next(it);
        while (proc != NULL){
            if (proc->pid != KERNEL_PID){
                int nameLenght = strlen(proc->name);
                printf(" %s  ", proc->name);
                if (nameLenght < 10){
                    for (int i = 0; i < 10 - nameLenght; i++){
                        printf(" ");
                    }
                }
                printf("%d       %d          %d         0x%x       0x%x", proc->pid, proc->priority, proc->foreground, proc->stackPointer, proc->stackBase);
                char state = proc->state;
                switch(state){
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
            proc = next(it);
        }
        freeIterator(it);
    }
}

void blockProcess(int pid) {
    Process process = getProcess(pid);
    process->state = BLOCKED;
    if (pid == scheduler->currentProcess->pid){
        scheduler->quantumCounter = scheduler->quantum;
        triggerTimer();
    }
}

void unblockProcess(int pid) {
    Process process = getProcess(pid);
    process->state = READY;
    if (scheduler->currentProcess->pid == EMPTY_PID){
        scheduler->quantumCounter = scheduler->quantum;
        triggerTimer();
    }
}

int getProcessState(int pid) {
    Process process = getProcess(pid);
    return process->state;
}

void killProcess() {
    printf("Killing process %s\n", scheduler->currentProcess->name);
    if (scheduler->currentProcess->pid == 0){
        Process kernel = getProcess(-1);
        kernel->state = READY;
        printf("Kernel ready\n");
    }
  //  printProcesses();
    // insert(scheduler->deleted, scheduler->currentProcess);
    // remove(scheduler->queue[scheduler->currentProcess->priority], scheduler->currentProcess);
    printf("Process killed\n");
    scheduler->currentProcess->state = ZOMBIE;
    yield();
  //  scheduler->quantumCounter = scheduler->quantum;
    triggerTimer();
}

void startWrapper(void* entryPoint, char argc, char * argv[]) {
    int ret = ((int (*)(int, char *[]))entryPoint)(argc, argv);
    killProcess();
}

void testList(){
    LinkedList list = createLinkedList();

    // Insert elements into the list
    int max = 100;
    int nums[max];
    for (int i = 0; i < max; i++) {
        nums[i] = i;
        insert(list, &nums[i]);
    }

    // Get the size of the list
    printf("List size: %d\n", getSize(list));

    // Iterate over the list and print the elements
    IteratorPtr it = iterator(list);
    while (hasNext(it)) {
        int* data = (int*)next(it);
        printf("Element: %d\n", *data);
    }
    freeIterator(it);

    // Remove an element from the list
    remove(list, &nums[1]);

    // Get an element at index
    int* element = (int*)get(list, 1);
    if (element != NULL)
        printf("Element at index 1: %d\n", *element);

    // Iterate over the updated list
    it = iterator(list);
    while (hasNext(it)) {
        int* data = (int*)next(it);
        printf("Updated Element: %d\n", *data);
    }
    freeIterator(it);

    // Destroy the list and free the memory
    destroyLinkedList(list);
    printf("List destroyed\n");
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
    process->priority = priority -1;
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
    
    if(process->pid == EMPTY_PID){
        scheduler->empty = process;
        return EMPTY_PID;
    }
    insert(scheduler->queue[process->priority], process);
    printf("Process %s created with pid %d at %x\n", process->name, process->pid, process);
    if (entryPoint == NULL){
        process->state = BLOCKED;
    }else{
        process->state = READY;
    }
    return process->pid;
}

