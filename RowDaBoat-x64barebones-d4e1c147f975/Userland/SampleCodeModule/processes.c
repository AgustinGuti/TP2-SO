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