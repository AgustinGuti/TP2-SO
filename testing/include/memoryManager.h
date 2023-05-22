#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define MIN_BLOCK_SIZE 32
#define BLOCK_STRUCT_SIZE 32

#define MANAGED_MEMORY_SIZE 0x10000

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(uint64_t managedMemorySize, void *const managedMemory, void *const memoryForMemoryManager, void *const memoryForManagerEnd);

void *allocMemory(MemoryManagerADT const memoryManager, const uint64_t memoryToAllocate);

uint64_t freeMemory(MemoryManagerADT const memoryManager, void *const memoryToFree);

uint64_t calculateRequiredMemoryManagerSize(uint64_t memoryToMap);

#endif
