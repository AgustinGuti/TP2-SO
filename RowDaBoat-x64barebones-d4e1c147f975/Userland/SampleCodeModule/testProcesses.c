#include <testProcesses.h>

int a = 100;
sem_t sem;

void processA() {
    // if (sem == NULL) {
    //     sem = semOpen("sem", 1);
    // }
    // semWait(sem);
    // if (a >= 100){
    // //    yield();
    //     a -= 100;
    // }
    // semPost(sem);
    // semClose(sem);
    // printf("Process A - PID: %d : %d\n", getpid(), a + 1);
    return;
}