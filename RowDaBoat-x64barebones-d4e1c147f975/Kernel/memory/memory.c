#include <memory.h>

MemoryManagerADT myMemoryManager;

MemoryManagerADT initializeMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForManager, uint64_t memoryForManagerEnd)
{
    myMemoryManager = createMemoryManager(size, (uint64_t *)initialDirection, (uint64_t *)memoryForManager, (uint64_t *)memoryForManagerEnd);
    return myMemoryManager;
}

static uint64_t freeMem;
static uint64_t occupiedMem = 0;
static uint64_t totalMem;

void *malloc(uint64_t size)
{
    void * res = allocMemory(myMemoryManager, size);
    if( res != NULL){
        occupiedMem += size;
    }
    return res;
}

uint64_t free(void *ptr)
{
    freeMemory(myMemoryManager, ptr);
}

void setTotalMemory(uint64_t totalMemForUse)
{
    totalMem = totalMemForUse;
    freeMem = totalMem;
}

uint64_t * getMemoryStatus()
{
    uint64_t * memoryStatus = malloc(3 * sizeof(uint64_t));
    if(memoryStatus == NULL){
        return NULL;
    }
    freeMem = totalMem - occupiedMem;
    memoryStatus[0] = totalMem;
    memoryStatus[1] = occupiedMem;
    memoryStatus[2] = freeMem;
    return memoryStatus;
}

