#include <testProcesses.h>
#include <stdio.h>

int a[1] = {100};
sem_t sem;

int processA() {
    // char foreground[2] = "1";
    // char *args[3] = {"processB", foreground, NULL};
    // pid_t pidB;
    // for (int i = 0; i < 1; i++)
    // {
    //     pidB = execve(&processB, args);
    //     waitpid(pidB);
    // }
    //printf("In A\n");
    // sem = semOpen("sem", 1);
    // semWait(sem);
    // if (*a >= 100){
    //    yield();
    //    *a -= 10;
    // }
    // semPost(sem);
    // semClose(sem);
    // for(int i= 0; i < 1000000000; i++){
    //     *a += 1;
    // }
    // printf("Process A - PID: %d : %d\n", getpid(), *a + 1);

    printf("%d\n", -1);
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