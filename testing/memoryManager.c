#include <memoryManager.h>
#include <stdlib.h>
#include <stdio.h>

#define FREE 0
#define OCCUPIED 1

#define MEMORY_MANAGER_STRUCT_SIZE 16
#define BLOCK_STRUCT_SIZE 32

typedef struct MemoryBlock
{
	void *startAddress;						 // Starting address of the memory block
	uint64_t size;								 // Size of the memory block in bytes
	struct MemoryBlock *nextBlock; // Pointer to the next free memory block
	struct MemoryBlock *prevBlock; // Pointer to the previous free memory block
} MemoryBlock;

typedef struct MemoryManagerCDT
{
	MemoryBlock *firstFreeBlock;		 // List of free memory blocks
	MemoryBlock *firstOccupiedBlock; // List of occupied memory blocks
} MemoryManagerCDT;

void initializeBlock(MemoryBlock *block, void *const startAddress, const uint64_t size);
void addBlockToOccupiedList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size);
void addBlockToFreeList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size);

MemoryManagerADT createMemoryManager(uint64_t managedMemorySize, void *const managedMemory, void *const memoryForMemoryManager, void *const memoryForManagerEnd)
{
	MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
	MemoryBlock *firstFreeBlock = (MemoryBlock *)(managedMemory);
	firstFreeBlock->startAddress = (uint64_t *)((uint64_t)managedMemory + BLOCK_STRUCT_SIZE);
	firstFreeBlock->size = managedMemorySize;
	firstFreeBlock->nextBlock = NULL;
	firstFreeBlock->prevBlock = NULL;
	memoryManager->firstFreeBlock = firstFreeBlock;
	memoryManager->firstOccupiedBlock = NULL;
	return memoryManager;
}

uint64_t calculateRequiredMemoryManagerSize(uint64_t memoryToMap)
{
	return MEMORY_MANAGER_STRUCT_SIZE;
}

void *allocMemory(MemoryManagerADT const memoryManager, const uint64_t memoryToAllocate)
{
	MemoryBlock *currentFreeBlock = memoryManager->firstFreeBlock;
	while (currentFreeBlock != NULL)
	{
		if (currentFreeBlock->size >= (memoryToAllocate + BLOCK_STRUCT_SIZE))
		{
			void *allocatedMemorystartAdress = (uint64_t *)((uint64_t)currentFreeBlock->startAddress + currentFreeBlock->size - memoryToAllocate);
			currentFreeBlock->size -= (memoryToAllocate + BLOCK_STRUCT_SIZE);

			if (currentFreeBlock->size == 0)
			{
				if (currentFreeBlock->prevBlock != NULL)
				{
					currentFreeBlock->prevBlock->nextBlock = currentFreeBlock->nextBlock;
				}
				else
				{
					memoryManager->firstFreeBlock = currentFreeBlock->nextBlock;
				}
				if (currentFreeBlock->nextBlock != NULL)
				{
					currentFreeBlock->nextBlock->prevBlock = currentFreeBlock->prevBlock;
				}
				currentFreeBlock = NULL;
			}
			addBlockToOccupiedList(memoryManager, allocatedMemorystartAdress, memoryToAllocate);
			// printBlocks(memoryManager);
			return (uint64_t *)((uint64_t)allocatedMemorystartAdress);
		}
		currentFreeBlock = currentFreeBlock->nextBlock;
	}
	return NULL;
}

uint64_t freeMemory(MemoryManagerADT const memoryManager, void *const memoryToFree)
{
	MemoryBlock *currentOccupiedBlock = memoryManager->firstOccupiedBlock;
	while (currentOccupiedBlock != NULL)
	{
		if (currentOccupiedBlock->startAddress == memoryToFree)
		{
			if (currentOccupiedBlock->prevBlock != NULL)
			{
				currentOccupiedBlock->prevBlock->nextBlock = currentOccupiedBlock->nextBlock;
			}
			else
			{
				memoryManager->firstOccupiedBlock = currentOccupiedBlock->nextBlock;
			}
			if (currentOccupiedBlock->nextBlock != NULL)
			{
				currentOccupiedBlock->nextBlock->prevBlock = currentOccupiedBlock->prevBlock;
			}
			addBlockToFreeList(memoryManager, memoryToFree, currentOccupiedBlock->size + BLOCK_STRUCT_SIZE);
			uint64_t size = currentOccupiedBlock->size;
			currentOccupiedBlock = NULL;
			// printBlocks(memoryManager);
			return size;
		}
		currentOccupiedBlock = currentOccupiedBlock->nextBlock;
	}
	return 0;
}

void addBlockToFreeList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size)
{
	MemoryBlock *currentBlock = memoryManager->firstFreeBlock;
	MemoryBlock *newBlock = (MemoryBlock *)((uint64_t)startAddress - BLOCK_STRUCT_SIZE);
	initializeBlock(newBlock, startAddress, size);
	if (currentBlock == NULL)
	{
		memoryManager->firstFreeBlock = newBlock;
		return;
	}
	while (currentBlock->nextBlock != NULL && currentBlock->startAddress < startAddress)
		currentBlock = currentBlock->nextBlock;

	// merge newBlock if adjacent
	if ((uint64_t)newBlock->startAddress + newBlock->size + BLOCK_STRUCT_SIZE == (uint64_t)currentBlock->startAddress)
	{
		newBlock->size += (currentBlock->size + BLOCK_STRUCT_SIZE);
		newBlock->nextBlock = currentBlock->nextBlock;
		newBlock->prevBlock = currentBlock->prevBlock;
		if (currentBlock->nextBlock != NULL)
			currentBlock->nextBlock->prevBlock = newBlock;
		if (currentBlock->prevBlock != NULL)
			currentBlock->prevBlock->nextBlock = newBlock;
		currentBlock = NULL;
		return;
	}
	if (currentBlock->prevBlock != NULL && (uint64_t)currentBlock->prevBlock->startAddress + currentBlock->prevBlock->size + BLOCK_STRUCT_SIZE == (uint64_t)newBlock->startAddress)
	{
		currentBlock->prevBlock->size += (newBlock->size + BLOCK_STRUCT_SIZE);
		newBlock = NULL;
		return;
	}
	newBlock->nextBlock = currentBlock;
	newBlock->prevBlock = currentBlock->prevBlock;
	if (currentBlock->prevBlock != NULL)
		currentBlock->prevBlock->nextBlock = newBlock;
	else
		memoryManager->firstFreeBlock = newBlock;
	currentBlock->prevBlock = newBlock;
}

void addBlockToOccupiedList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size)
{
	// printBlocks(memoryManager);
	MemoryBlock *currentBlock = memoryManager->firstOccupiedBlock;
	MemoryBlock *newBlock = (MemoryBlock *)((uint64_t)startAddress - BLOCK_STRUCT_SIZE);
	initializeBlock(newBlock, startAddress, size);
	if (currentBlock == NULL)
	{
		memoryManager->firstOccupiedBlock = newBlock;
		return;
	}
	while (currentBlock->nextBlock != NULL && currentBlock->startAddress < startAddress)
		currentBlock = currentBlock->nextBlock;
	newBlock->nextBlock = currentBlock;
	newBlock->prevBlock = currentBlock->prevBlock;
	if (currentBlock->prevBlock != NULL)
		currentBlock->prevBlock->nextBlock = newBlock;
	else
		memoryManager->firstOccupiedBlock = newBlock;
	currentBlock->prevBlock = newBlock;
}

void initializeBlock(MemoryBlock *block, void *const startAddress, const uint64_t size)
{
	block->startAddress = startAddress;
	block->size = size;
	block->nextBlock = NULL;
	block->prevBlock = NULL;
}

void printBlocks(MemoryManagerADT const memoryManager)
{
	printf("\n");
	printf("Free blocks:\n");
	printf("\n");
	MemoryBlock *currentFreeBlock = memoryManager->firstFreeBlock;
	while (currentFreeBlock != NULL)
	{
		printf("Free Block Start address: %x, size: %x\n", currentFreeBlock->startAddress, currentFreeBlock->size);
		currentFreeBlock = currentFreeBlock->nextBlock;
	}
	printf("\n");
	printf("Occupied blocks:\n");
	printf("\n");
	MemoryBlock *currentOccupiedBlock = memoryManager->firstOccupiedBlock;
	while (currentOccupiedBlock != NULL)
	{
		printf("Occupied Block Start address: %x, size: %x\n", currentOccupiedBlock->startAddress, currentOccupiedBlock->size);
		currentOccupiedBlock = currentOccupiedBlock->nextBlock;
	}
}
