#include <sysCallInterface.h>

sem_t semOpen(char *name, int value);
void semClose(sem_t sem);
void semWait(sem_t sem);
void semPost(sem_t sem);
