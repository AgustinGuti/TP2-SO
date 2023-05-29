#include <stdint.h>
#include "memoryManager.h"


MemoryManagerADT initializeMemoryManager(uint64_t size, void* initialDirection, void* memoryForManager, void* memoryForManagerEnd);
void *malloc(uint64_t size);
uint64_t free(void *ptr);
void * realloc(void *ptr, uint64_t newSize);
void setTotalMemory(uint64_t totalMem);
uint64_t * getMemoryStatus();