#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#define MIN_BLOCK_SIZE 32

typedef struct BuddyCDT *BuddyADT;

uint64_t calculateRequiredBuddySize(uint64_t memoryToMap);

BuddyADT init_buddy(uint64_t size, uint64_t initialDirection, uint64_t memoryForBuddy, uint64_t memoryForBuddyEnd);

void* allocMemory(BuddyADT buddy, uint64_t size);

uint64_t freeMemory(BuddyADT buddy, void *ptr);


#endif
