#include <testProcesses.h>

void processA() {
    for (int i = 0; i < 5; i++){
        printf("In process A\n", 0);
        yield();
    }
    // printf("Process A PID: %d\n", 1, getpid());
    printf("Process A returning\n", 0);
    exit(0);
}

void processB() {
    for (int i = 0; i < 5; i++){
        printf("In process B\n", 0);
        yield();
    }
    // printf("Process B PID: %d\n", 1, getpid());
    printf("Process B returning\n", 0);
    exit(0);
}