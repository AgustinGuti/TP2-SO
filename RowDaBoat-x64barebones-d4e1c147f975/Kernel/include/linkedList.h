#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include "memory.h"


typedef struct NodeCTD * Node;
typedef struct LinkedListCDT * LinkedList;
typedef struct IteratorCDT* IteratorPtr;


// Function declarations
LinkedList createLinkedList();
void destroyLinkedList(LinkedList list);
void insert(LinkedList list, void* data);
void remove(LinkedList list, void* data);
void * get(LinkedList list, int index);
IteratorPtr iterator(LinkedList list);
int hasNext(IteratorPtr iterator);
void * next(IteratorPtr iterator);
int getSize(LinkedList list);
void freeIterator(IteratorPtr iterator);
void headToBack(LinkedList list);
void resetIterator(IteratorPtr iterator, LinkedList list);
void moveToBack(LinkedList list, void* data);

#endif /* LINKED_LIST_H */
