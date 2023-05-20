#include <testProcesses.h>

void processA() {
    for (int i = 0; i < 1; i++){
        yield();
    }
    printf("Process A PID: %d\n", 1, getpid());
    printf("Process A returning\n", 0);
    exit(0);
}

void processB() {
    for (int i = 0; i < 0; i++){
        yield();
    }
    printf("Process B PID: %d\n", 1, getpid());
    printf("Process B returning\n", 0);
    exit(0);
}