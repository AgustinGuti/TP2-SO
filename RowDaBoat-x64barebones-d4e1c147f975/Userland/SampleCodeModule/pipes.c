#include <pipes.h>

Pipe pipe(char * name, int fds[2]) {
    return _sys_openProcessPipe(name, fds);
}

int close(int fd) {
    return _sys_closeProcessPipe(fd);
}