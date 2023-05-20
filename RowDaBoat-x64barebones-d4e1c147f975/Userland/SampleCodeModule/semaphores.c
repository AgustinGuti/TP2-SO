#include <semaphores.h>

sem_t semOpen(char *name, int value){
    return _sys_semOpen(name, value);
}
void semClose(sem_t sem){
    _sys_semClose(sem);
}
void semWait(sem_t sem){
    _sys_semWait(sem);
}
void semPost(sem_t sem){
    _sys_semPost(sem);
}
