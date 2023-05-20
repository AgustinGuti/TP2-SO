#include <testProcesses.h>

void processA() {
    for (int i = 0; i < 100000000; i++){}
    printf("Process A\n", 0);
    return 0;
}

void processB() {
    for (int i = 0; i < 100000000; i++){}
    printf("Process B\n", 0);
    return 0;
}