// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
