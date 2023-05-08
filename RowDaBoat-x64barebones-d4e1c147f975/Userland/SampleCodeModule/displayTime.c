#include <stdio.h>
#include <sysCallInterface.h>

#define UTC -3

void displayTime() {
    int hour, min, seg;

    _sys_getTime(&hour, &min, &seg);
    
    hour += UTC;
    printf("%x:%x:%x\n", 3, hour, min, seg);
}

