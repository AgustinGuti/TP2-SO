#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include "memory.h"


typedef struct NodeCTD * Node;
typedef struct LinkedListCDT * LinkedList;


// Function declarations
LinkedList createLinkedList();
void destroyLinkedList(LinkedList list);
void insert(LinkedList list, void* data);
void remove(LinkedList list, void* data);
void * get(LinkedList list, int index);
void * iterator(LinkedList list);
int hasNext(void * iterator);
void * next(void * iterator);
void * getData(void * iterator);
int getSize(LinkedList list);


#endif /* LINKED_LIST_H */
