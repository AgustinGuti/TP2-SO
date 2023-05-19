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
void removeItem(LinkedList list, void* data);

#endif /* LINKED_LIST_H */
