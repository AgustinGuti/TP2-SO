#include <displayTime.h>
#include <stdio.h>
#include <sysCallInterface.h>

#define UTC -3

char displayTime(char argc, char **argv)
{
    int hour, min, seg;

    _sys_getTime(&hour, &min, &seg);

    hour += UTC;
    printf("%x:%x:%x\n", hour, min, seg);
    return 0;
}
