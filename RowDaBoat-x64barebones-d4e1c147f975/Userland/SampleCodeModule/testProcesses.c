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
    for(int i= 0; i < 1000000000; i++){
        *a += 1;
    }
    printf("Process A - PID: %d : %d\n", getpid(), *a + 1);
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
    /* cat process implementation*/
    close(0);
    int fds[2];
    if( pipe("pipe", fds) == -1){
        printf("Error creating pipe\n");
        return;
    }
    printf("fd0: %d , fd1: %d\n", fds[0], fds[1]);
    while(1){
        char buffer[100];
        int read = _sys_read(fds[0], buffer, 100);
        if(read != 0)
            _sys_write(1, buffer, read);
        printf("read: %d\n", read);
    }
    close(fds[0]);
    close(fds[1]);
}