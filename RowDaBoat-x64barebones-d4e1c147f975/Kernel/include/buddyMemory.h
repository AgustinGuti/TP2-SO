#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>
#include <stdint.h>


typedef struct BuddyCDT *BuddyADT;

uint64_t calculateRequiredBuddySize(int memoryToMap, uint16_t minBlockSize);

BuddyADT init_buddy(int size, uint64_t initialDirection, uint64_t memoryForBuddy, uint64_t memoryForBuddyEnd, uint16_t minBlockSize);

void* allocateMemory(BuddyADT buddy, uint64_t size);

uint64_t freeMemory(BuddyADT buddy, void *address);


#endif
