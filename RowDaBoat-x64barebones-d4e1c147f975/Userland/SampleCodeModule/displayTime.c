// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
