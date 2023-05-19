#include <testProcesses.h>


void processA() {
    printf("Process A\n", 0);
    return;
}

void processB() {
    printf("Process B\n", 0);
    while(1);
}