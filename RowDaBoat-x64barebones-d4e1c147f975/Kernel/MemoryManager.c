#include "MemoryManager.h"
#include <lib.h>

#define FREE 0
#define OCCUPIED 1

void addBlockToList(MemoryManagerADT const memoryManager, void *const startAddress, const size_t size, uint8_t listType);

typedef struct MemoryBlock
{
	void *startAddress; // Starting address of the memory block
	uint64_t size;			// Size of the memory block in bytes
	void *nextBlock;		// Pointer to the next free memory block
	void *prevBlock;		// Pointer to the previous free memory block
} MemoryBlock;

typedef struct MemoryManagerCDT
{
	MemoryBlock *firstFreeBlock;		 // List of free memory blocks
	MemoryBlock *firstOccupiedBlock; // List of occupied memory blocks
} MemoryManagerCDT;

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory, uint64_t managedMemorySize)
{
	MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
	MemoryBlock firstFreeBlock;
	firstFreeBlock.startAddress = managedMemory;
	firstFreeBlock.size = managedMemorySize;
	firstFreeBlock.nextBlock = NULL;
	firstFreeBlock.prevBlock = NULL;
	memoryManager->firstFreeBlock = &firstFreeBlock;
	memoryManager->firstOccupiedBlock = NULL;
	return memoryManager;
}

void *allocMemory(MemoryManagerADT const memoryManager, const size_t memoryToAllocate)
{
	MemoryBlock *currentFreeBlock = memoryManager->firstFreeBlock;
	while (currentFreeBlock != NULL)
	{
		if (currentFreeBlock->size >= memoryToAllocate)
		{
			void *allocatedMemory = currentFreeBlock->startAddress;
			currentFreeBlock->startAddress += memoryToAllocate;
			currentFreeBlock->size -= memoryToAllocate;

			if (currentFreeBlock->size == 0)
			{
				if (currentFreeBlock->prevBlock != NULL)
				{
					currentFreeBlock->prevBlock->nextBlock = currentFreeBlock->nextBlock;
				}
				if (currentFreeBlock->nextBlock != NULL)
				{
					currentFreeBlock->nextBlock->prevBlock = currentFreeBlock->prevBlock;
				}
			}
			addBlockToList(memoryManager, allocatedMemory, memoryToAllocate, OCCUPIED);
			return allocatedMemory;
		}
		currentFreeBlock = currentFreeBlock->nextBlock;
	}
	return NULL;
}

void freeMemory(MemoryManagerADT const memoryManager, void *const memoryToFree)
{
	MemoryBlock *currentOccupiedBlock = memoryManager->firstOccupiedBlock;
	while (currentOccupiedBlock != NULL)
	{
		if (currentOccupiedBlock->startAddress == memoryToFree)
		{
			if (currentOccupiedBlock->nextBlock != NULL)
			{
				currentOccupiedBlock->prevBlock->nextOccupiedBlock = currentOccupiedBlock->nextBlock;
			}
			if (currentOccupiedBlock->prevBlock != NULL)
				currentOccupiedBlock->nextBlock->prevOccupiedBlock = currentOccupiedBlock->prevBlock;
			addBlockToList(memoryManager, memoryToFree, currentOccupiedBlock->size, FREE);
			return;
		}
		currentOccupiedBlock = currentOccupiedBlock->nextBlock;
	}
}

void addBlockToList(MemoryManagerADT const memoryManager, void *const startAddress, const size_t size, uint8_t listType)
{
	MemoryBlock *currentBlock = listType == FREE ? memoryManager->firstFreeBlock : memoryManager->firstOccupiedBlock;
	MemoryBlock *newBlock;
	newBlock->startAddress = startAddress;
	newBlock->size = size;
	newBlock->nextBlock = NULL;
	newBlock->prevBlock = NULL;
	if (currentBlock == NULL)
	{
		if (listType == FREE)
			memoryManager->firstFreeBlock = newBlock;
		else
			memoryManager->firstOccupiedBlock = newBlock;
		return;
	}
	while (currentBlock->nextBlock != NULL && currentBlock->startAddress < startAddress)
		currentBlock = currentBlock->nextBlock;
	// merge newBlock if adjacent
	if (listType == FREE)
	{
		if (newBlock->startAddress + newBlock->size == currentBlock->startAddress)
		{
			newBlock->size += currentBlock->size;
			newBlock->nextBlock = currentBlock->nextBlock;
			newBlock->prevBlock = currentBlock->prevBlock;
			if (currentBlock->nextBlock != NULL)
				currentBlock->nextBlock->prevBlock = newBlock;
			if (currentBlock->prevBlock != NULL)
				currentBlock->prevBlock->nextBlock = newBlock;
			return;
		}
		if (currentBlock->prevBlock != NULL && currentBlock->prevBlock->startAddress + currentBlock->prevBlock->size == newBlock->startAddress)
		{
			currentBlock->prevBlock->size += newBlock->size;
			newBlock = NULL;
			return;
		}
	}

	newBlock->nextBlock = currentBlock;
	newBlock->prevBlock = currentBlock->prevBlock;
	if (currentBlock->prevBlock != NULL)
		currentBlock->prevBlock->nextBlock = newBlock;
	currentBlock->prevBlock = newBlock;
}

// typedef struct MemoryManagerCDT
// {
// 	char *nextAddress;
// } MemoryManagerCDT;

// MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory)
// {
// 	MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
// 	memoryManager->nextAddress = managedMemory;

// 	return memoryManager;
// }
/*
void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate)
{
	char *allocation = memoryManager->nextAddress;

	memoryManager->nextAddress += memoryToAllocate;

	return (void *)allocation;
}
*/