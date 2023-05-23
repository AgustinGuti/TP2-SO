#include <testProcesses.h>


int a[1] = {100};
sem_t sem;

void processA() {
    while(1){
        printf("A");
        yield();
    }
    return;
}

void processB() {
    printf("B");
    blockProcess(1);
    return;
}