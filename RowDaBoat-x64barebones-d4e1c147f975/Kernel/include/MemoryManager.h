#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#define MIN_BLOCK_SIZE 32

#define MANAGED_MEMORY_SIZE 0x10000

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForBuddy, uint64_t memoryForBuddyEnd);

void *allocMemory(MemoryManagerADT const memoryManager, const size_t memoryToAllocate);

uint64_t freeMemory(MemoryManagerADT const memoryManager, void *ptr);

#endif
