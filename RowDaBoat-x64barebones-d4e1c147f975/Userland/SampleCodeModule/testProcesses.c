#include <testProcesses.h>


int a[1] = {100};
sem_t sem;

void processA() {
    // sem_t sem = semOpen("test", 1);
    // semWait(sem);
    if (*a >= 100){
        yield();
        *a -= 100;
    }
   // semPost(sem);
    printf("Process A PID: %d A: %d\n", getpid(), *a+1);
    exit(0);
}


void processB() {
    // sem_t sem = semOpen("test", 1);
    // semWait(sem);
    if (*a >= 100){
        yield();
        *a -= 100;
    }
   // semPost(sem);
    printf("Process B PID: %d A: %d\n", getpid(), *a+1);
    return;
}