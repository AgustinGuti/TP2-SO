#include <testProcesses.h>
#include <stdio.h>

int a[1] = {100};
sem_t sem;

int processA() {
  
    sem = semOpen("sem", 1);
    semWait(sem);
    if (*a >= 100){
       yield();
       *a -= 10;
    }
    semPost(sem);
    semClose(sem);
    printf("Process A - PID: %d : %d\n", getpid(), *a + 1);
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