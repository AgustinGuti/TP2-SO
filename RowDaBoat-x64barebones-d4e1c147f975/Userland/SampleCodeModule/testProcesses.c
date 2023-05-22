#include <testProcesses.h>


int a[1] = {100};
sem_t sem;

void processA() {
  //  sem_t sem = semOpen("test", 1);
 //   for(int i = 0; i < 10; i++){
   //     semWait(sem);
    //     if (*a >= 100){
    //    //     yield();
    //         *a -= 100;
    //     }
    //     if (*a != 0){
    //         printf("-----------------------ERROR-----------------------");
    //     }
    //  //   semPost(sem);
    //     printf("Process A PID: %d A: %d\n", getpid(), *a+1);
    // //    semWait(sem);
    //     if (*a < 100){
    //       //  yield();
    //         *a += 100;
    //     }
    //     if (*a != 100){
    //         printf("-----------------------ERROR-----------------------");
    //     }
     //   semPost(sem);
    //    yield();
  //  }
   // semClose(sem);
    return;
}


void processB() {
    sem_t sem = semOpen("test", 1);
    for(int i = 0; i < 100; i++){
        semWait(sem);
        if (*a >= 100){
            yield();
            *a -= 100;
        }
        if (*a != 0){
            printf("-----------------------ERROR-----------------------");
        }
        semPost(sem);
        printf("Process B PID: %d A: %d\n", getpid(), *a+1);
        semWait(sem);
        if (*a < 100){
            yield();
            *a += 100;
        }
        if (*a != 100){
            printf("-----------------------ERROR-----------------------");
        }
        semPost(sem);
    }
    semClose(sem);
    return;
}