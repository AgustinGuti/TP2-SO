#include <testProcesses.h>


int a = 100;
sem_t sem;

void processA() {
    //blockProcess(getpid());
    sem_t sem = semOpen("test", 1);
    semWait(sem);
    if (a >= 100){
        yield();
        a -= 100;
    }
    semPost(sem);
    // for (int i = 0; i < 10; i++){
    //     yield();
    // }

    printf("Process A PID: %d A: %d\n", 2, getpid(), a+1);
    exit(0);
}

void processB() {
    sem_t sem = semOpen("test", 1);
    semWait(sem);
    if (a >= 100){
        yield();
        a -= 100;
    }
    semPost(sem);
    // for (int i = 0; i < 10; i++){
    //     yield();
    // }
    printf("Process B PID: %d A: %d\n", 2, getpid(), a+1);
    return;
}