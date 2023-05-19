#include "MemoryManager.h"
#include <lib.h>
#include <videoDriver.h>

#define FREE 0
#define OCCUPIED 1
#define NOT_USED 0
#define USED 1

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

void printBlocks(MemoryManagerADT const memoryManager);
void initializeBlock(MemoryBlock *block, void *const startAddress, const uint64_t size, uint64_t index);
void addBlockToOccupiedList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size);
void addBlockToFreeList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size);
uint8_t getBlockState(uint64_t *memory, uint64_t bit);
void setBlockUsed(uint64_t *memory, uint64_t bit);
void setBlockNotUsed(uint64_t *memory, uint64_t bit);
uint64_t getFirstNotUsedBlock(uint64_t *memory);

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

MemoryManagerADT createMemoryManager(uint64_t managedMemorySize, void *const managedMemory, void *const memoryForMemoryManager, void *const memoryForManagerEnd)
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
			// printf("Allocating memory in direction %x\n", 1, currentFreeBlock->startAddress);
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
			addBlockToOccupiedList(memoryManager, allocatedMemorystartAdress, memoryToAllocate);
			// printBlocks(memoryManager);
			return allocatedMemorystartAdress;
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
			if (currentOccupiedBlock->nextBlock != NULL)
			{
				currentOccupiedBlock->prevBlock->nextBlock = currentOccupiedBlock->nextBlock;
			}
			if (currentOccupiedBlock->prevBlock != NULL)
				currentOccupiedBlock->nextBlock->prevBlock = currentOccupiedBlock->prevBlock;
			printf("Adding block to Free list\n", 0);
			addBlockToFreeList(memoryManager, memoryToFree, currentOccupiedBlock->size);
			setBlockNotUsed(memoryManager->occupiedBlocksState, currentOccupiedBlock->index);
			uint64_t size = currentOccupiedBlock->size;
			currentOccupiedBlock = NULL;
			// printBlocks(memoryManager);
			return size;
		}
		currentOccupiedBlock = currentOccupiedBlock->nextBlock;
	}
	return 0;
}

void printBlocks(MemoryManagerADT const memoryManager)
{
	printf("\n", 0);
	printf("Free blocks:\n", 0);
	printf("\n", 0);
	MemoryBlock *currentFreeBlock = memoryManager->firstFreeBlock;
	while (currentFreeBlock != NULL)
	{
		printf("Free Block Index: %d Start address: %x, size: %x\n", 3, currentFreeBlock->index, currentFreeBlock->startAddress, currentFreeBlock->size);
		currentFreeBlock = currentFreeBlock->nextBlock;
	}
	printf("\n", 0);
	printf("Occupied blocks:\n", 0);
	printf("\n", 0);
	MemoryBlock *currentOccupiedBlock = memoryManager->firstOccupiedBlock;
	while (currentOccupiedBlock != NULL)
	{
		printf("Occupied Block Index: %d Start address: %x, size: %x\n", 3, currentOccupiedBlock->index, currentOccupiedBlock->startAddress, currentOccupiedBlock->size);
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

void addBlockToFreeList(MemoryManagerADT const memoryManager, void *const startAddress, const uint64_t size)
{
	MemoryBlock *currentBlock = memoryManager->firstFreeBlock;
	uint64_t index = getFirstNotUsedBlock(memoryManager->freeBlocksState);
	MemoryBlock *newBlock = (MemoryBlock *)((uint64_t *)memoryManager + MEMORY_MANAGER_STRUCT_SIZE + index * BLOCK_STRUCT_SIZE);
	setBlockUsed(memoryManager->freeBlocksState, index);
	initializeBlock(newBlock, startAddress, size, index);
	if (currentBlock == NULL)
	{
		memoryManager->firstFreeBlock = newBlock;
		return;
	}
	while (currentBlock->nextBlock != NULL && currentBlock->startAddress < startAddress)
		currentBlock = currentBlock->nextBlock;

	// merge newBlock if adjacent
	if (newBlock->startAddress + newBlock->size == currentBlock->startAddress)
	{
		newBlock->size += currentBlock->size;
		newBlock->nextBlock = currentBlock->nextBlock;
		newBlock->prevBlock = currentBlock->prevBlock;
		if (currentBlock->nextBlock != NULL)
			currentBlock->nextBlock->prevBlock = newBlock;
		if (currentBlock->prevBlock != NULL)
			currentBlock->prevBlock->nextBlock = newBlock;
		setBlockNotUsed(memoryManager->freeBlocksState, currentBlock->index);
		currentBlock = NULL;
		return;
	}
	if (currentBlock->prevBlock != NULL && currentBlock->prevBlock->startAddress + currentBlock->prevBlock->size == newBlock->startAddress)
	{
		currentBlock->prevBlock->size += newBlock->size;
		setBlockNotUsed(memoryManager->freeBlocksState, newBlock->index);
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
	MemoryBlock *currentBlock = memoryManager->firstOccupiedBlock;
	uint64_t index = getFirstNotUsedBlock(memoryManager->occupiedBlocksState);
	// printf("New occupied block index: %d\n", 1, index);
	MemoryBlock *newBlock = (MemoryBlock *)((uint64_t *)memoryManager + MEMORY_MANAGER_STRUCT_SIZE + BLOCK_QUANTITY * BLOCK_STRUCT_SIZE + index * BLOCK_STRUCT_SIZE);
	setBlockUsed(memoryManager->occupiedBlocksState, index);
	initializeBlock(newBlock, startAddress, size, index);
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

void initializeBlock(MemoryBlock *block, void *const startAddress, const uint64_t size, uint64_t index)
{
	block->startAddress = startAddress;
	block->size = size;
	block->index = index;
	block->nextBlock = NULL;
	block->prevBlock = NULL;
}