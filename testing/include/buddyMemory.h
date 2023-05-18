#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#define MIN_BLOCK_SIZE 32

typedef struct MemoryManagerCDT *MemoryManagerADT;

uint64_t calculateRequiredBuddySize(uint64_t memoryToMap);

MemoryManagerADT createMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForBuddy, uint64_t memoryForBuddyEnd);

void* allocMemory(MemoryManagerADT buddy, uint64_t size);

uint64_t freeMemory(MemoryManagerADT buddy, void *address);


#endif
