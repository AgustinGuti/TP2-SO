#include <processes.h>

int fork(){
    return _sys_fork();
}

int execve(void* entryPoint, char *const argv[]){
    return _sys_execve(entryPoint, argv);
}

void printProcesses(){
    _sys_printProcesses();
}

int getpid(){
    return _sys_getpid();
}

void yield(){
    _sys_yield();
}

void exit(int value){
    _sys_exit(value);
}

void blockProcess(int pid){
    _sys_block(pid);
}

void kill(int pid){
    _sys_kill(pid);
}

pid_t waitpid(pid_t pid){
    return _sys_waitpid(pid);
}