#include <testProcesses.h>
#include <stdio.h>

int a = 100;
sem_t sem;

int processA() {
    // if (sem == NULL) {
    //     sem = semOpen("sem", 1);
    // }
    // semWait(sem);
    // printf("Yielding %d\n", getpid());
    // if (a >= 100){
    //     yield();
    //     a -= 100;
    // }
    // printf("Resuming %d\n", getpid());
    // semPost(sem);
    // printf("posted %d\n", getpid());
    // semClose(sem);
    printf("Process A - PID: %d : %d\n", getpid(), a + 1);
    return 0;
}

// void cat() {
//   char c;
// //   while( (getChar(c)) != EOF)
//   while(1) {
//     c = getChar();
//     if(c != NULL)
//         putChar(c);
//   }
//   return;
// }