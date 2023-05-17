#include <buddyMemory copy.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// Necesita 256 MB para mapear los 64 GB de memoria, con bloques de 32 bytes

#define BUDDY_STRUCT_SIZE 24

#define RIGHT(x) ((x)*2 + 1+1)
#define LEFT(x) ((x)*2+1)
#define PARENT(x) ((x-1)/2)
#define BROTHER(x) ((x)%2 == 0 ? (x)+1 : (x)-1) ///RECALCULAR

#define OCCUPIED 1
#define FREE 0


// estructura para representar el buddy allocator
typedef struct BuddyCDT {
    uint64_t initialDirection;
    uint32_t size; // tamaño de la memoria total
    uint32_t neededBlocks;
    // uint64_t memory[neededBlocks/64]
    uint64_t *memory;  //0 representa que está libre, 1 que está ocupado, es un arreglo que ocupa desde su posicion hasta el final de la posicion asignada al buddy
} BuddyCDT;


uint64_t getBlockState(uint64_t *memory, uint64_t bit) {
    return (memory[bit/64] >> (bit%64)) & 1;
}

uint64_t occupyBlock(uint64_t *memory, uint64_t bit) {
    memory[bit/64] |= 1UL << (bit%64);
}

uint64_t freeBlock(uint64_t *memory, uint64_t bit) {
    memory[bit/64] &= ~(1UL << (bit%64));
}

uint64_t getOrder(uint64_t bit){
    uint64_t order = 0;
    while (bit > 0) {
        bit = bit/2;
        order++;
    }
    return order;
}

uint64_t getBlockSize(BuddyADT buddy, uint64_t bit) {
    uint64_t order = getOrder(bit);
    uint64_t block_size = buddy->size >> order;
    return block_size;
}

uint64_t getBlockStart(BuddyADT buddy, uint64_t bit) {
    uint64_t order = getOrder(bit);
    uint64_t block_size = buddy->size >> order;
    uint64_t block_start = block_size * (bit - (1 << order));
    return block_start;
}

uint64_t getBlockIndex(BuddyADT  buddy, void* address) {
    uint64_t bit = 0;
    uint64_t currentAddress = buddy->initialDirection;
    uint64_t order = getOrder(bit);
    uint64_t block_size = buddy->size >> order;
    uint64_t block_start = 0;
    while (block_size > 0) {
        block_size = block_size >> 1;   // block_size /= 2
        if (address >= (void*) (currentAddress + block_start + block_size)) {
            block_start += block_size;
            bit = RIGHT(bit);
        } else {
            bit = LEFT(bit);
        }
    }
    //Found the bit at the last level
    return bit;
}

uint64_t calculateRequiredBuddySize(uint64_t memoryToMap, uint16_t minBlockSize){
    uint64_t buddySize = 0;
    uint64_t memoryToMapInBlocks = memoryToMap / minBlockSize;
    uint64_t memoryToMapInBlocksInBytes = (memoryToMapInBlocks / 64) * 8;
    buddySize = memoryToMapInBlocksInBytes + BUDDY_STRUCT_SIZE;
    return buddySize;
}

BuddyADT init_buddy(int size, uint64_t initialDirection, uint64_t memoryForBuddyStart, uint64_t memoryForBuddyEnd, uint16_t minBlockSize) {
    if (memoryForBuddyEnd - memoryForBuddyStart < calculateRequiredBuddySize(size, minBlockSize)){
        return NULL;
    }
    BuddyADT buddy = (BuddyADT)memoryForBuddyStart;
    buddy->size = size;
    buddy->neededBlocks = (size/minBlockSize) * 2 - 1; //Suma de potencias de 2 hasta size es 2**(size+1) - 1
    buddy->initialDirection = initialDirection;
    buddy->memory = (uint64_t*) (memoryForBuddyStart + BUDDY_STRUCT_SIZE);
    memset(buddy->memory, 0, ceil((double)buddy->neededBlocks/8));
    return buddy;
}

void* allocateMemory(BuddyADT buddy, uint64_t size) {
    uint64_t bit = 0;
    while (bit < buddy->neededBlocks && (getBlockState(buddy->memory, bit) == OCCUPIED || getBlockSize(buddy, bit) < size)) {
        if (getBlockSize(buddy, LEFT(bit)) >= size) {
            bit = LEFT(bit);
        } else if (getBlockSize(buddy, RIGHT(bit)) >= size) {
            bit = RIGHT(bit);
        } else {
            bit = PARENT(bit);
        }
    }
    
    if (bit >= buddy->neededBlocks) {
        // No suitable block found
        return NULL;
    }
    
    while (getBlockSize(buddy, bit) / 2 > size) {
        if (getBlockState(buddy->memory, LEFT(bit)) == FREE) {
            occupyBlock(buddy->memory, LEFT(bit));
            occupyBlock(buddy->memory, bit);
            bit = LEFT(bit);
        } else if (getBlockState(buddy->memory, RIGHT(bit)) == FREE) {
            occupyBlock(buddy->memory, RIGHT(bit));
            occupyBlock(buddy->memory, bit);
            bit = RIGHT(bit);
        } 
    }
    
    // Mark the block as allocated and return a pointer to its start address
    occupyBlock(buddy->memory, bit);
    return (void*)(buddy->initialDirection + getBlockStart(buddy, bit));
}

uint64_t freeMemory(BuddyADT buddy, void *address) {
    if (address == NULL){
        return 0;
    }
    // Get the index of the block that contains the memory pointed to by address
    uint64_t bit = getBlockIndex(buddy, address);
    
    // Traverse up the tree until we find a block that has a set bit (i.e., allocated memory)
    while (bit > 0 && bit < buddy->neededBlocks && getBlockState(buddy->memory, bit) == 0) {
        bit = PARENT(bit);
    }
    
    // Mark the block as free
    freeBlock(buddy->memory, bit);
    
    // Traverse up the tree, clearing parent nodes that have no siblings that are allocated
    while (bit > 0 && getBlockState(buddy->memory, BROTHER(bit)) == 0) {
        bit = PARENT(bit);
        freeBlock(buddy->memory, bit);
    }
    
    // Return the amount of bytes freed
    return getBlockSize(buddy, bit);
}
