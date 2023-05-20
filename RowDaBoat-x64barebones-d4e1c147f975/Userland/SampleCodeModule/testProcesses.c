#include <testProcesses.h>

void processA() {
    for (int i = 0; i < 100; i++){
        yield();
    }
    printf("Process A PID: %d\n", 1, getpid());
    exit(0);
}

void processB() {
    for (int i = 0; i < 100; i++){
        yield();
    }
    printf("Process B PID: %d\n", 1, getpid());
    return;
}