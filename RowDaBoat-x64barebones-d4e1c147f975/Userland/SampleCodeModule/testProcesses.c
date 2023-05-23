#include <testProcesses.h>


int a[1] = {100};
sem_t sem;

void processA() {
    for (int i = 0; i < 3; i++){
        printf("A, %d\n", getpid());
        yield();
    }
    return;
}