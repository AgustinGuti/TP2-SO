#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <lib.h>
#include <stddef.h>
#include <stdint.h>

#define MIN_BLOCK_SIZE 32

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(uint64_t managedMemorySize, void *const managedMemory, void *const memoryForMemoryManager, void *const memoryForManagerEnd);

void *allocMemory(MemoryManagerADT const memoryManager, const uint64_t memoryToAllocate, uint64_t *allocatedMemorySize);

uint64_t freeMemory(MemoryManagerADT const memoryManager, void *const memoryToFree);

void *reallocMemory(MemoryManagerADT const memoryManager, void *const memoryToRealloc, const uint64_t newSize, int64_t *const allocatedMemorySize);

uint64_t calculateRequiredMemoryManagerSize(uint64_t memoryToMap);

#endif
