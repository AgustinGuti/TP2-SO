#include "linkedList.h"
#include <videoDriver.h>

// Structure representing a node in the linked list
typedef struct NodeCDT {
    void* data;
    struct NodeCDT* next;
}NodeCDT;

// Structure representing the linked list
typedef struct LinkedListCDT {
    struct NodeCDT* head;
    struct NodeCDT* tail;
    uint8_t size;
}LinkedListCDT;

struct Iterator {
    struct NodeCDT* current;
};

LinkedList createLinkedList() {
    struct LinkedListCDT * list = (LinkedList)malloc(sizeof(struct LinkedListCDT));
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
    return list;
}

void destroyLinkedList(LinkedList list) {
    if (list == NULL)
        return;

    struct NodeCDT * current = list->head;
    while (current != NULL) {
        Node next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

//new node is inserted at the end of the list
void insert(LinkedList list, void* data) {
    if (list == NULL)
        return;

    struct NodeCDT * node = (struct NodeCDT*)malloc(sizeof(struct NodeCDT));
    if (node == NULL)
        return;

    node->data = data;
    node->next = NULL;

    if (list->size == 0) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
}

//removes the first item in the list that contains data
void remove(LinkedList list, void* data) {
    if (list == NULL || list->size == 0)
        return;

    struct NodeCDT* current = list->head;
    struct NodeCDT* previous = NULL;
  //  printf("list size antes: %d\n",list->size);
    while (current != NULL) {
        if (current->data == data) {
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            list->size--;
            //printf("list size despues: %d\n",list->size);

            return;
        }

        previous = current;
        current = current->next;
    }
  //  printf("list size despues: %d\n",list->size);
}

//returns the data of the node at index
void * get(LinkedList list, int index) {
    if (list == NULL || index < 0 || index >= list->size)
        return NULL;

    struct NodeCDT* current = list->head;
    int i = 0;
    while (i < index) {
        current = current->next;
        i++;
    }

    return current->data;
}

IteratorPtr iterator(LinkedList list) {
    if (list == NULL)
        return NULL;

    IteratorPtr iterator = (IteratorPtr)malloc(sizeof(struct Iterator));
    if (iterator == NULL)
        return NULL;

    iterator->current = list->head;
    return iterator;
}

void* next(IteratorPtr iterator) {
    if (iterator == NULL || !hasNext(iterator))
        return NULL;

    struct NodeCDT* node = iterator->current;
    iterator->current = node->next;
    return node->data;
}

int hasNext(IteratorPtr iterator) {
    if (iterator == NULL)
        return 0;

    return iterator->current != NULL;
}

void freeIterator(IteratorPtr iterator) {
    free(iterator);
}

int getSize(LinkedList list) {
    if (list == NULL)
        return -1;

    return list->size;
}
