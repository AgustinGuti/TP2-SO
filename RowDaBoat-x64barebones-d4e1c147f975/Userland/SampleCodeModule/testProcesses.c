#include <testProcesses.h>
#include <stdio.h>

int a[1] = {100};
sem_t sem;

int processA() {
    _sys_sleep(10000);
    return 0;
}

void processB() {
    char foreground[2] = "1";
    char *args[3] = {"processC", foreground, NULL};
    pid_t pidC;
    for (int i = 0; i < 2; i++)
    {
        pidC = execve(&processC, args);
        waitpid(pidC);
    }
    printf("In B\n");
    // while(1);
    return ;
}

void processC() {
    printf("In C\n");
    return;
}

void cat() {
    char buffer[2];
    char c;
    while((c=getChar()) != EOF){
        printf("%c", c);
    }
}

void wc() {
    int count = 1;
    char c;
    while ((c = getChar()) != EOF) {
        printf("%c", c);
        if (c == '\n') {
            count++;
        }
    }
    printf("\nLineas: %d\n", count);
}