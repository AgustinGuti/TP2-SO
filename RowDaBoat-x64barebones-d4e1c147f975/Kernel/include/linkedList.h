#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>
#include "memory.h"

typedef struct NodeCTD * Node;
typedef struct LinkedListCDT * LinkedList;
typedef struct IteratorCDT* Iterator;


// Function declarations
LinkedList createLinkedList();
void destroyLinkedList(LinkedList list);
void insert(LinkedList list, void* data);
void remove(LinkedList list, void* data);
void * get(LinkedList list, int index);
Iterator iterator(LinkedList list);
int hasNext(Iterator iterator);
void * next(Iterator iterator);
int getSize(LinkedList list);
void freeIterator(Iterator iterator);
void headToBack(LinkedList list);
void resetIterator(Iterator iterator);
void moveToBack(LinkedList list, void* data);
void printList(LinkedList list);
void * removeFirst(LinkedList list);
void switchList(LinkedList origin, LinkedList dest);
void * findItem(LinkedList list, void *data, int (*comparator)(void *, void *));

#endif /* LINKED_LIST_H */
