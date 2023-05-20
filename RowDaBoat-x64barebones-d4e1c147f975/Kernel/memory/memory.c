#include <memory.h>

MemoryManagerADT myMemoryManager;

MemoryManagerADT initializeMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForManager, uint64_t memoryForManagerEnd)
{
    myMemoryManager = createMemoryManager(size, (uint64_t *)initialDirection, (uint64_t *)memoryForManager, (uint64_t *)memoryForManagerEnd);
    return myMemoryManager;
}

void *malloc(uint64_t size)
{
    return allocMemory(myMemoryManager, size);
}

uint64_t free(void *ptr)
{
    freeMemory(myMemoryManager, ptr);
}
