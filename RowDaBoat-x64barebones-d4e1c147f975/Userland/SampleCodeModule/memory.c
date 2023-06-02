// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memory.h>
#include <stdint.h>

void *malloc(uint64_t size)
{
    return _sys_malloc(size);
}

uint64_t free(void *ptr)
{
    return _sys_free(ptr);
}

uint64_t *getMemoryStatus()
{
    return _sys_getMemoryStatus();
}

void *realloc(void *ptr, uint64_t newSize)
{
    return _sys_realloc(ptr, newSize);
}