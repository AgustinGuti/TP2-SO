#include <processes.h>

int execve(void* entryPoint, Pipe *pipes, char pipeQty, char *const argv[]){
    return _sys_execve(entryPoint, pipes, pipeQty, argv);
}

void printProcesses(char argc, char **argv){
    if (argc == 0){
        _sys_printProcesses(0);
    }
    else if (argc == 1 && strcmp(argv[0], "-k") == 0){
        _sys_printProcesses(1);
    }
    else{
        printf("Invalid arguments\n");
    }
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