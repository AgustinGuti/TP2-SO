// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <pipes.h>

Pipe pipe(char * name, int fds[2]) {
    return _sys_openProcessPipe(name, fds);
}

int close(int fd) {
    return _sys_closeProcessPipe(fd);
}