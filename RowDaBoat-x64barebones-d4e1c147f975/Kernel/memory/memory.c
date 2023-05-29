#include <memory.h>
#include <memoryManager.h>
MemoryManagerADT myMemoryManager;

static uint64_t occupiedMem = 0;
static uint64_t totalMem;

MemoryManagerADT initializeMemoryManager(uint64_t size, void* initialDirection, void* memoryForManager, void* memoryForManagerEnd)
{
    totalMem = size;
    myMemoryManager = createMemoryManager(size, initialDirection, memoryForManager, memoryForManagerEnd);
    return myMemoryManager;
}

static uint64_t memoryStatus[3];

void *malloc(uint64_t size)
{
    uint64_t allocatedMemory = size;
    void * res = allocMemory(myMemoryManager, size, &allocatedMemory);
    if( res != NULL){
        occupiedMem += allocatedMemory;
    }else {
        printerr("Malloc failed\n");
    }
    return res;
}

uint64_t free(void *ptr)
{
    freeMemory(myMemoryManager, ptr);
}

void * realloc(void *ptr, uint64_t newSize)
{
    uint64_t allocatedMemory = newSize;
    void * res = reallocMemory(myMemoryManager, ptr, newSize, &allocatedMemory);
    if( res != NULL){
        occupiedMem += allocatedMemory;
    }else {
        printerr("Realloc failed\n");
    }
    return res;
}


uint64_t * getMemoryStatus()
{
    memoryStatus[0] = totalMem;
    memoryStatus[1] = occupiedMem;
    memoryStatus[2] = totalMem - occupiedMem;
    return memoryStatus;
}

