#include <stdint.h>
#include <memoryManager.h>

MemoryManagerADT initializeMemoryManager(uint64_t size, uint64_t initialDirection, uint64_t memoryForManager, uint64_t memoryForManagerEnd);
void *myMalloc(uint64_t size);
uint64_t myFree(void *ptr);
void printBlocks(MemoryManagerADT const memoryManager);
