#include <stdint.h>
#include "memoryManager.h"


MemoryManagerADT initializeMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForManager, uint64_t memoryForManagerEnd);
void *malloc(uint64_t size);
uint64_t free(void *ptr);
void setTotalMemory(uint64_t totalMem);
uint64_t * getMemoryStatus();