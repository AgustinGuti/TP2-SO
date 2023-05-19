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

void insert(struct LinkedListCDT* list, void* data) {
    if (list == NULL)
        return;

    struct NodeCDT * newNode = (Node)malloc(sizeof(struct NodeCDT));
    if (newNode == NULL)
        return;

    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    list->size++;
}

void removeItem(LinkedList list, void* data) {
    if (list == NULL || list->head == NULL)
        return;

    struct NodeCDT* current = list->head;
    struct NodeCDT* previous = NULL;

    while (current != NULL) {
        if (current->data == data) {
            if (previous == NULL) {
                list->head = current->next;
                if (list->head == NULL)
                    list->tail = NULL;
            } else {
                previous->next = current->next;
                if (current->next == NULL)
                    list->tail = previous;
            }

            free(current);
            list->size--;
            return;
        }

        previous = current;
        current = current->next;
    }
}

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

void * iterator(LinkedList list) {
    if (list == NULL)
        return NULL;

    return list->head;
}

void * next(void * iterator) {
    if (iterator == NULL)
        return NULL;

    struct NodeCDT* node = (struct NodeCDT*)iterator;
    return node->next;
}

int hasNext(void * iterator) {
    if (iterator == NULL)
        return 0;

    struct NodeCDT* node = (struct NodeCDT*)iterator;
    return node->next != NULL;
}

void * getData(void * iterator) {
    if (iterator == NULL)
        return NULL;

    struct NodeCDT* node = (struct NodeCDT*)iterator;
    return node->data;
}

int getSize(LinkedList list) {
    if (list == NULL)
        return -1;

    return list->size;
}
