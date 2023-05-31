#include <memoryManager.h>

#define MAX_MEMORY 0x1000000000     //64 GB mapped in pure64


#include <lib.h>
#include <math.h>
#include <videoDriver.h>

// Necesita 256 MB para mapear los 64 GB de memoria, con bloques de 32 bytes

#define BUDDY_STRUCT_SIZE 24

#define RIGHT(x) ((x)*2 + 1+1)
#define LEFT(x) ((x)*2+1)
#define PARENT(x) ((x-1)/2)
#define BROTHER(x) ((x)%2 == 0 ? (x)-1 : (x)+1)


#define OCCUPIED 1
#define FREE 0


// estructura para representar el buddy allocator
typedef struct MemoryManagerCDT {
    void * initialDirection;
    uint32_t size; // tamaño de la memoria total
    uint32_t neededBlocks;
    // uint64_t memory[neededBlocks/64]
    uint8_t *memory;  //0 representa que está libre, 1 que está ocupado, es un arreglo que ocupa desde su posicion hasta el final de la posicion asignada al buddy
} MemoryManagerCDT;

uint64_t alignMemoryToBlock(uint64_t size);


uint8_t getBlockState(uint8_t *memory, uint64_t bit) {
    return (memory[bit/8] >> (bit%8)) & 1;
}

void occupyBlock(uint8_t *memory, uint64_t bit) {
    memory[bit/8] |= 1UL << (bit%8);
}

void freeBlock(uint8_t *memory, uint64_t bit) {
    memory[bit/8] &= ~(1UL << (bit%8));
}

uint8_t getOrder(uint64_t bit){
    uint8_t order = 0;
    while (bit > 0) {
        bit = (bit-1)/2;
        order++;
    }
    return order;
}

uint64_t getBlockSize(MemoryManagerADT buddy, uint64_t bit) {
    uint64_t order = getOrder(bit);
    uint64_t block_size = buddy->size >> order;
    return block_size;
}

void* getBlockStart(MemoryManagerADT buddy, uint64_t bit) {
    uint64_t order = getOrder(bit);
    uint64_t block_size = buddy->size >> order;
    void * block_start = block_size * ((bit + 1) - (1 << order));
    return block_start;
}

long getBlockIndex(MemoryManagerADT  buddy, void* address) {
    uint64_t bit = 0;
    void* currentAddress = buddy->initialDirection;
    uint64_t block_size = buddy->size;
    void* block_start = 0;
    if (address < (void*) currentAddress || address >= (void*) (currentAddress + buddy->size)) {
        return -1;
    }
    while (block_size > MIN_BLOCK_SIZE) {
        block_size = block_size >> 1;   // block_size /= 2
        if (address >= (void*) ((uint64_t)currentAddress + (uint64_t)block_start + block_size)) {
            block_start += block_size;
            bit = RIGHT(bit);
        } else {
            bit = LEFT(bit);
        }
    }
    //Found the bit at the last level
    return bit;
}

uint64_t calculateRequiredMemoryManagerSize(uint64_t memoryToMap){
    uint64_t buddySize = 0;
    uint64_t memoryToMapAligned = alignMemoryToBlock(memoryToMap);
    uint64_t memoryToMapInBlocks = (memoryToMapAligned / MIN_BLOCK_SIZE) * 2 - 1;
    uint64_t memoryToMapInBytes = (memoryToMapInBlocks / 8) ;
    buddySize = memoryToMapInBytes + BUDDY_STRUCT_SIZE;
    return buddySize;
}


MemoryManagerADT createMemoryManager(uint64_t managedMemorySize, void *const managedMemory,  void *const  memoryForMemoryManager, void *const memoryForManagerEnd) {
    uint64_t memoryToMapAligned = alignMemoryToBlock(managedMemorySize);
    if (memoryForManagerEnd - memoryForMemoryManager < calculateRequiredMemoryManagerSize(memoryToMapAligned)){
        return NULL;
    }
    if (memoryToMapAligned + managedMemory > MAX_MEMORY || memoryToMapAligned + managedMemory < managedMemory){
        printerr("No se puede mapear mas de 64 GB de memoria");
        return NULL;
    }
    MemoryManagerADT buddy = (MemoryManagerADT)memoryForMemoryManager;
    buddy->size = memoryToMapAligned;
    buddy->neededBlocks = (memoryToMapAligned/MIN_BLOCK_SIZE) * 2 - 1; //Suma de potencias de 2 hasta size es 2**(size+1) - 1
    buddy->initialDirection = managedMemory;
    buddy->memory = (uint8_t*) (memoryForMemoryManager + BUDDY_STRUCT_SIZE);
    memset(buddy->memory, 0, (buddy->neededBlocks)/8);
    return buddy;
}

void printTree(MemoryManagerADT buddy, uint8_t order){
    // print the first order levels of binary tree  in buddy->memory
    int i;
    uint64_t bit = 0;
    for (i = 0; i < order; i++) {
        printf("Level %d: ", i+1);
        while (bit < buddy->neededBlocks && getOrder(bit) == i) {
            if (getBlockState(buddy->memory, bit) == OCCUPIED) {
                printf("1 ");
            } else {
                printf("0 ");
            }
            bit++;
        }
        printf("\n");
    }
    printf("\n");
}

uint64_t alignMemoryToBlock(uint64_t size){
    // Aligns the size to the previous power of 2
    uint64_t alignedSize = MIN_BLOCK_SIZE;
    while (alignedSize <= size && alignedSize <= UINT64_MAX/2) {
        alignedSize *= 2;
    }
    alignedSize /= 2;
    return alignedSize;
}

void* allocMemoryRec(MemoryManagerADT buddy, uint64_t size, uint64_t bit, uint64_t *allocatedMemorySize){
    uint64_t block_size = getBlockSize(buddy, bit);
    if (block_size < size) {
        return NULL;
    }
    if (block_size >= 2 * size && RIGHT(bit) < buddy->neededBlocks && !(getBlockState(buddy->memory, bit) == OCCUPIED && getBlockState(buddy->memory, RIGHT(bit)) == FREE && getBlockState(buddy->memory, LEFT(bit)) == FREE)){ //RIGHT is further than LEFT

        void* left = allocMemoryRec(buddy, size, LEFT(bit), allocatedMemorySize);
        if (left != NULL){
            occupyBlock(buddy->memory, bit);
            return left;
        }
        void* right = allocMemoryRec(buddy, size, RIGHT(bit), allocatedMemorySize);
        if (right != NULL) {
            occupyBlock(buddy->memory, bit);
            return right;
        }

        return NULL;
    }
    if (getBlockState(buddy->memory, bit) == OCCUPIED){
        return NULL;
    }
    occupyBlock(buddy->memory, bit);
    if (allocatedMemorySize != NULL){
        *allocatedMemorySize = block_size;
    }
    return (void*) ((uint64_t)buddy->initialDirection + (uint64_t)getBlockStart(buddy, bit));
}

void printMemoryState(){

}

void* allocMemory(MemoryManagerADT buddy, uint64_t size, uint64_t *allocatedMemorySize) {
    void* res= allocMemoryRec(buddy, size, 0, allocatedMemorySize);
    
    // printf("Allocated %x bytes at 0x%x\n", size, res);
    // long bit = getBlockIndex(buddy, res);
    // if (bit < 0) {
    //     return 0;
    // }
    // // Traverse up the tree until we find a block that has a set bit (i.e., allocated memory)
    // while (bit > 0  && getBlockState(buddy->memory, bit) == 0) {
    //     bit = PARENT(bit);
    // }

    // uint64_t block_size = getBlockSize(buddy, bit);

    // printf("Allocated between %x and %x\n", getBlockStart(buddy, bit), getBlockStart(buddy, bit) + block_size - 1);
    //printTree(buddy, 6);
    return res;
}

uint64_t freeMemory(MemoryManagerADT buddy, void *ptr) {
    if (ptr == NULL){
        return 0;
    }
    // Get the index of the block that contains the memory pointed to by address
    long bit = getBlockIndex(buddy, ptr);
    if (bit < 0) {
        return 0;
    }
    // Traverse up the tree until we find a block that has a set bit (i.e., allocated memory)
    while (bit > 0  && getBlockState(buddy->memory, bit) == 0) {
        bit = PARENT(bit);
    }

    uint64_t block_size = getBlockSize(buddy, bit);
    // Mark the block as free
    freeBlock(buddy->memory, bit);
    
    // Traverse up the tree, clearing parent nodes that have no siblings that are allocated
    while (bit > 0 && getBlockState(buddy->memory, BROTHER(bit)) == 0) {
        bit = PARENT(bit);
        freeBlock(buddy->memory, bit);
    }
    //printTree(buddy, 6);
    // Return the amount of bytes freed
  //  printf("Freeing %d bytes at 0x%x\n", block_size, ptr);

    return block_size;
}
