#include "MemoryManager.h"
#include <lib.h>

#define FREE 0
#define OCCUPIED 1
#define NOT_USED 0
#define USED 1

void addBlockToList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size, uint8_t listType);

#define MEMORY_MANAGER_STRUCT_SIZE 16 + 8 * BLOCK_QUANTITY / 64
#define BLOCK_STRUCT_SIZE 32
#define BLOCK_QUANTITY 1000
#define BLOCKS_LIST__SIZE BLOCK_STRUCT_SIZE *BLOCK_QUANTITY

typedef struct MemoryBlock
{
	void *startAddress;						 // Starting address of the memory block
	uint64_t size;								 // Size of the memory block in bytes
	struct MemoryBlock *nextBlock; // Pointer to the next free memory block
	struct MemoryBlock *prevBlock; // Pointer to the previous free memory block
	uint64_t index;
} MemoryBlock;

typedef struct MemoryManagerCDT
{
	MemoryBlock *firstFreeBlock;		 // List of free memory blocks
	MemoryBlock *firstOccupiedBlock; // List of occupied memory blocks
	uint64_t freeBlocksState[BLOCK_QUANTITY / 64];
	uint64_t occupiedBlocksState[BLOCK_QUANTITY / 64];
} MemoryManagerCDT;

uint8_t getBlockState(uint64_t *memory, uint64_t bit)
{
	return (memory[bit / 64] >> (bit % 64)) & 1;
}

void setBlockUsed(uint64_t *memory, uint64_t bit)
{
	memory[bit / 64] |= 1UL << (bit % 64);
}

void setBlockNotUsed(uint64_t *memory, uint64_t bit)
{
	memory[bit / 64] &= ~(1UL << (bit % 64));
}

MemoryManagerADT createMemoryManager(void *const memoryForMemoryManager, void *const managedMemory, uint64_t managedMemorySize)
{
	MemoryManagerADT memoryManager = (MemoryManagerADT)memoryForMemoryManager;
	MemoryBlock *firstFreeBlock = (MemoryBlock *)((uint64_t *)memoryForMemoryManager + MEMORY_MANAGER_STRUCT_SIZE);
	firstFreeBlock->startAddress = managedMemory;
	firstFreeBlock->size = managedMemorySize;
	firstFreeBlock->index = 0;
	firstFreeBlock->nextBlock = NULL;
	firstFreeBlock->prevBlock = NULL;
	memoryManager->firstFreeBlock = firstFreeBlock;
	memoryManager->firstOccupiedBlock = NULL;
	setBlockUsed(memoryManager->freeBlocksState, firstFreeBlock->index);
	return memoryManager;
}

uint64_t calculateRequiredMemoryManagerSize(uint64_t memoryToMap)
{
	return MEMORY_MANAGER_STRUCT_SIZE + BLOCKS_LIST__SIZE * 2;
}

void *allocMemory(MemoryManagerADT const memoryManager, const uint64_t memoryToAllocate)
{
	MemoryBlock *currentFreeBlock = memoryManager->firstFreeBlock;
	while (currentFreeBlock != NULL)
	{
		if (currentFreeBlock->size >= memoryToAllocate)
		{
			void *allocatedMemorystartAdress = currentFreeBlock->startAddress;
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
				setBlockNotUsed(memoryManager->freeBlocksState, currentFreeBlock->index);
				currentFreeBlock = NULL;
			}
			addBlockToList(memoryManager, allocatedMemorystartAdress, memoryToAllocate, OCCUPIED);
			return allocatedMemorystartAdress;
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
				currentOccupiedBlock->prevBlock->nextBlock = currentOccupiedBlock->nextBlock;
			}
			if (currentOccupiedBlock->prevBlock != NULL)
				currentOccupiedBlock->nextBlock->prevBlock = currentOccupiedBlock->prevBlock;
			addBlockToList(memoryManager, memoryToFree, currentOccupiedBlock->size, FREE);
			return;
		}
		currentOccupiedBlock = currentOccupiedBlock->nextBlock;
	}
}

uint64_t getFirstNotUsedBlock(uint64_t *memory)
{
	uint64_t bit = 0;
	uint64_t index = 0;
	while (memory[index] == 0xFFFFFFFFFFFFFFFF)
		index++;
	bit = index * 64;
	while (getBlockState(memory, bit) == USED)
		bit++;
	return bit;
}

void addBlockToList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size, uint8_t listType)
{
	MemoryBlock *currentBlock = listType == FREE ? memoryManager->firstFreeBlock : memoryManager->firstOccupiedBlock;
	uint64_t index = getFirstNotUsedBlock(memoryManager->freeBlocksState);
	MemoryBlock *newBlock;
	if (listType == FREE)
	{
		newBlock = (MemoryBlock *)((uint64_t *)memoryManager + MEMORY_MANAGER_STRUCT_SIZE + index * BLOCK_STRUCT_SIZE);
		setBlockUsed(memoryManager->freeBlocksState, index);
	}
	else
	{
		newBlock = (MemoryBlock *)((uint64_t *)memoryManager + MEMORY_MANAGER_STRUCT_SIZE + BLOCK_QUANTITY * BLOCK_STRUCT_SIZE + index * BLOCK_STRUCT_SIZE);
		setBlockUsed(memoryManager->occupiedBlocksState, index);
	}
	newBlock->startAddress = startAddress;
	newBlock->size = size;
	newBlock->index = index;
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
			setBlockNotUsed(memoryManager->freeBlocksState, newBlock->index);
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
