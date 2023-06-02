// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#include <lib.h>
#include <videoDriver.h>

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

void printBlocks(MemoryManagerADT const memoryManager);
void initializeBlock(MemoryBlock *block, void *const startAddress, const uint64_t size);
void addBlockToOccupiedList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size);
void addBlockToFreeList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size);

MemoryManagerADT createMemoryManager(uint64_t managedMemorySize, void *const managedMemory, void *const memoryForMemoryManager, void *const memoryForManagerEnd)
{
	MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
	MemoryBlock *firstFreeBlock = (MemoryBlock *)(managedMemory);
	initializeBlock(firstFreeBlock, managedMemory, managedMemorySize - BLOCK_STRUCT_SIZE);
	memoryManager->firstFreeBlock = firstFreeBlock;
	memoryManager->firstOccupiedBlock = NULL;
	return memoryManager;
}

uint64_t calculateRequiredMemoryManagerSize(uint64_t memoryToMap)
{
	return MEMORY_MANAGER_STRUCT_SIZE;
}

void *allocMemory(MemoryManagerADT const memoryManager, const uint64_t memoryToAllocate, uint64_t *allocatedMemorySize)
{
	MemoryBlock *currentFreeBlock = memoryManager->firstFreeBlock;
	while (currentFreeBlock != NULL)
	{
		if (currentFreeBlock->size >= (memoryToAllocate + BLOCK_STRUCT_SIZE))
		{
			void *allocatedMemorystartAddress = (uint64_t *)((uint64_t)currentFreeBlock->startAddress + currentFreeBlock->size - memoryToAllocate);
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
			}
			addBlockToOccupiedList(memoryManager, allocatedMemorystartAddress, memoryToAllocate);

			if (allocatedMemorySize != NULL)
			{
				*allocatedMemorySize = memoryToAllocate + BLOCK_STRUCT_SIZE;
			}
			return (uint64_t *)((uint64_t)allocatedMemorystartAddress);
		}
		currentFreeBlock = currentFreeBlock->nextBlock;
	}
	printBlocks(memoryManager);
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
			uint64_t size = currentOccupiedBlock->size;
			addBlockToFreeList(memoryManager, memoryToFree, currentOccupiedBlock->size);
			return size;
		}
		currentOccupiedBlock = currentOccupiedBlock->nextBlock;
	}
	return 0;
}

void *reallocMemory(MemoryManagerADT const memoryManager, void *const memoryToRealloc, const uint64_t newSize, int64_t *const allocatedMemorySize)
{
	if (memoryToRealloc == NULL)
	{
		return NULL;
	}
	void *newMemory = allocMemory(memoryManager, newSize, allocatedMemorySize);
	if (newMemory == NULL)
	{
		return NULL;
	}
	MemoryBlock *currentBlock = memoryManager->firstOccupiedBlock;
	while (currentBlock != NULL)
	{
		if (currentBlock->startAddress == memoryToRealloc)
		{
			break;
		}
		currentBlock = currentBlock->nextBlock;
	}
	if (currentBlock == NULL)
	{
		return NULL;
	}
	memcpy(newMemory, memoryToRealloc, currentBlock->size < newSize ? currentBlock->size : newSize);
	*allocatedMemorySize -= currentBlock->size;
	freeMemory(memoryManager, memoryToRealloc);
	return newMemory;
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
	MemoryBlock *prevBlock = NULL;

	while (currentBlock != NULL && currentBlock->startAddress < startAddress)
	{
		prevBlock = currentBlock;
		currentBlock = currentBlock->nextBlock;
	}

	if (currentBlock == NULL)
	{
		prevBlock->nextBlock = newBlock;
		newBlock->prevBlock = prevBlock;
		return;
	}

	newBlock->prevBlock = prevBlock;
	if (prevBlock != NULL)
		prevBlock->nextBlock = newBlock;
	else
		memoryManager->firstFreeBlock = newBlock;
	newBlock->nextBlock = currentBlock;
	currentBlock->prevBlock = newBlock;

	// merge blocks
	if (prevBlock != NULL && (uint64_t)prevBlock->startAddress + prevBlock->size == (uint64_t)newBlock)
	{
		prevBlock->size += newBlock->size + BLOCK_STRUCT_SIZE;
		prevBlock->nextBlock = newBlock->nextBlock;
		if (newBlock->nextBlock != NULL)
		{
			newBlock->nextBlock->prevBlock = prevBlock;
		}
		newBlock = prevBlock;
	}
	if (newBlock->nextBlock != NULL && (uint64_t)newBlock->startAddress + newBlock->size == (uint64_t)newBlock->nextBlock)
	{
		newBlock->size += newBlock->nextBlock->size + BLOCK_STRUCT_SIZE;
		newBlock->nextBlock = newBlock->nextBlock->nextBlock;
		if (newBlock->nextBlock != NULL)
		{
			newBlock->nextBlock->prevBlock = newBlock;
		}
	}
}

void addBlockToOccupiedList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size)
{
	MemoryBlock *currentBlock = memoryManager->firstOccupiedBlock;
	MemoryBlock *newBlock = (MemoryBlock *)((uint64_t)startAddress - BLOCK_STRUCT_SIZE);
	initializeBlock(newBlock, startAddress, size);
	if (currentBlock == NULL)
	{
		memoryManager->firstOccupiedBlock = newBlock;
		return;
	}
	MemoryBlock *prevBlock = NULL;

	while (currentBlock != NULL && currentBlock->startAddress < startAddress)
	{
		prevBlock = currentBlock;
		currentBlock = currentBlock->nextBlock;
	}

	if (currentBlock == NULL)
	{
		prevBlock->nextBlock = newBlock;
		newBlock->prevBlock = prevBlock;
		return;
	}

	newBlock->prevBlock = prevBlock;
	if (prevBlock != NULL)
		prevBlock->nextBlock = newBlock;
	else
		memoryManager->firstOccupiedBlock = newBlock;
	newBlock->nextBlock = currentBlock;
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
	printf("DONE\n");
}
