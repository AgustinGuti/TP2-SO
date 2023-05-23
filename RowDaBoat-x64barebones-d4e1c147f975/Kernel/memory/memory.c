#include <memory.h>

MemoryManagerADT myMemoryManager;

static uint64_t occupiedMem = 0;
static uint64_t totalMem;

MemoryManagerADT initializeMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForManager, uint64_t memoryForManagerEnd)
{
    totalMem = size;
    myMemoryManager = createMemoryManager(size, (uint64_t *)initialDirection, (uint64_t *)memoryForManager, (uint64_t *)memoryForManagerEnd);
    return myMemoryManager;
}

static uint64_t memoryStatus[3];

void *malloc(uint64_t size)
{
    uint64_t allocatedMemory = size;
    void * res = allocMemory(myMemoryManager, size, &allocatedMemory);
    if( res != NULL){
        occupiedMem += allocatedMemory;
    }
    return res;
}

uint64_t free(void *ptr)
{
    freeMemory(myMemoryManager, ptr);
}

uint64_t * getMemoryStatus()
{
    memoryStatus[0] = totalMem;
    memoryStatus[1] = occupiedMem;
    memoryStatus[2] = totalMem - occupiedMem;
    return memoryStatus;
}

