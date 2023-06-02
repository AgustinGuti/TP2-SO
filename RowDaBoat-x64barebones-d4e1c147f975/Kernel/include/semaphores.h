#include <scheduler.h>
#include <functions.h>
#include <linkedList.h>


typedef struct semaphoreCDT* sem_t;

// Creates or opens a semaphore and identifies it with the name "name".
// If a semaphore with that name does not exist, it creates it with value "value".
// If a semaphore with that name already exists, it opens it and ignores the value "value".
sem_t semOpen(char *name, int value);

// Closes the semaphore identified by sem.
// If there are no processes waiting on the semaphore, it kills it.
// If there are processes waiting on the semaphore, it does not kill it.
void semClose(sem_t sem);

// Decrements the value of the semaphore identified by sem by one.
// If the value of the semaphore is zero, the calling process is blocked.
void semWait(sem_t sem);

// Increases the value of the semaphore identified by sem by one.
// If there are blocked processes in the semaphore, unblock one of them.
void semPost(sem_t sem);