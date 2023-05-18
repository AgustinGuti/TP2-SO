#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

#define MANAGED_MEMORY_SIZE 0x10000

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory);

void *allocMemory(MemoryManagerADT const memoryManager, const size_t memoryToAllocate);

#endif
