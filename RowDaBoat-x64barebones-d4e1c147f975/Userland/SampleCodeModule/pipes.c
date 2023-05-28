#include <pipes.h>

Pipe pipe(char * name) {
    return _sys_openPipe(name);
}

int closePipe(Pipe pipe) {
    // return _sys_closePipe(pipe);
}