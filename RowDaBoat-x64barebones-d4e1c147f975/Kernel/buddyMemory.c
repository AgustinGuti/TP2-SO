#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define MIN_BLOCK_SIZE 4096 // tamaño de bloque mínimo

#define MAX_MEMORY 0x1000000000     //64 GB mapped in pure64


#define BUDDY_TOTAL_SIZE (( MAX_MEMORY - 0x400000) / MIN_BLOCK_SIZE) * BLOCK_STRUCT_SIZE + BUDDY_STRUCT_SIZE

#define BUDDY_STRUCT_SIZE 28
#define BLOCK_STRUCT_SIZE 37

// estructura para representar un bloque de memoria
typedef struct block {
    uint64_t initialDirection;
    uint32_t size; // tamaño del bloque
    uint8_t free; // indica si el bloque está libre
    struct block *parent; // padre del bloque en el árbol binario
    struct block *right; 
    struct block *left; 
} block_t;

// estructura para representar el buddy allocator
typedef struct buddy {
    uint64_t initialDirection;
    uint64_t blocksCreated;
    uint32_t size; // tamaño de la memoria total
    block_t *root; // raíz del árbol binario
} buddy_t;

//  0        24     72   120
///  buddy |  b1   | b2 | b3 | b4 | b5 | b6 | b7 | b8 | b9 | b10

// función para inicializar un bloque
block_t *init_block(int size, uint64_t memoryForBlock) {
    block_t *b = (block_t*)memoryForBlock;
    b->size = size;
    b->free = 1;
    b->right = NULL;
    b->left = NULL;
    b->parent = NULL;
    return b;
}


buddy_t *init_buddy(int size, uint64_t initialDirection, uint64_t memoryForBuddy) {
    buddy_t *buddy = (buddy_t*)memoryForBuddy;
    buddy->size = size;
    buddy->initialDirection = initialDirection;
    buddy->blocksCreated = 0;
    buddy->root = init_block(size, memoryForBuddy + BUDDY_STRUCT_SIZE);
    return buddy;
}

// función para asignar un bloque de memoria
block_t *allocate_block(buddy_t *buddy, int size) {
    // encuentra el bloque libre más adecuado en el árbol binario
    block_t *block = buddy->root;
    if (buddy->root == NULL || buddy->size < size) {
        return NULL;
    }
    // navega por los niveles del arbol
    while (block != NULL && (!block->free || block->size < size)) {
        if (block->left != NULL && block->left->size >= size) {
            block = block->left;
        } else if (block->right != NULL && block->right->size >= size) {
            block = block->right;
        } else {
            block = block->parent;
        }
    }

    // si no se encuentra un bloque libre adecuado, devuelve NULL
    if (block == NULL || !block->free || block->size < size) {
        return NULL;
    }

    // divide el bloque en dos bloques más pequeños si es necesario
    while (block->size / 2 >= size) {
        block->left = create_block(block->size / 2);
        block->right = create_block(block->size / 2);
        block->left->free = 1;
        block->right->free = 1;
        block->left->parent = block;
        block->right->parent = block;
        block->free = 0;
        block = block->left;
    }

    block->free = 0; // marca el bloque como asignado
    return block;
}


// función para liberar un bloque de memoria
void free_block(buddy_t *buddy, block_t *block) {
    // marca el bloque como libre
    block->free = 1;

    // une los bloques libres
    while (block->parent != NULL && block->parent->left->free && block->parent->right->free) {
        //block->parent->left = NULL;
        //block->parent->right = NULL;
        block->parent->free = 1;
        block = block->parent;
    }
}


// función para liberar un bloque de memoria
void free_block(buddy_t *buddy, block_t *block) {
    block->free = 1; // marca el bloque como libre

    // fusiona los bloques y actualiza el estado de liberación
    block = merge_blocks(block);

    // actualiza los bloques padre si es necesario
    while (block->parent != NULL) {
        block = block->parent;
        block = merge_blocks(block);
    }

    // agrega el bloque liberado a la raíz del árbol binario
    block->left = NULL;
    block->right = NULL;

    if (buddy->root != NULL) {
        buddy->root->parent = block;
        block->left = buddy->root;
    }

    buddy->root = block;
}

// función auxiliar para fusionar dos bloques
block_t *merge_blocks(block_t *block) {
    // fusiona los bloques hermanos si están libres y tienen el mismo tamaño
    while (block->parent != NULL && block->free && block->parent->left->free && block->parent->right->free && block->size == block->parent->left->size && block->size == block->parent->right->size) {
        block = block->parent;
        block->free = 1;
        block->left->free = 0;
        block->right->free = 0;
    }
    return block;
}



/* 
block_t *findFreeBlock(block_t  *block, uint64_t size){
    if (size > block->size || !block->free) {
        return NULL;
    }
    if (block->size / 2 >= size)  {
        if (block->left == NULL){
            block_t blockLeft;
            init_block(&blockLeft, block->size / 2);
            return &blockLeft;
        }else if (block->left->free){
            return findFreeBlock(block->left, size);
        }else if (block->right == NULL){
            block_t blockRight;
            init_block(&blockRight, block->size / 2);
            return &blockRight;
        }else if (block->right->free){
            return findFreeBlock(block->right, size);
        }else{
            return NULL;
        }
    }
}
*/