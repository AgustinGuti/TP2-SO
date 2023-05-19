#include "linkedList.h"

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
    MemoryManagerADT memoryManager;
}LinkedListCDT;


LinkedList createLinkedList(MemoryManagerADT memoryManager) {
    struct LinkedListCDT * list = (LinkedList)allocMemory(memoryManager, sizeof(struct LinkedListCDT));
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
        list->memoryManager = memoryManager;
    }
    return list;
}

void destroyLinkedList(LinkedList list) {
    if (list == NULL)
        return;

    struct NodeCDT * current = list->head;
    while (current != NULL) {
        Node next = current->next;
        freeMemory(list->memoryManager, current);
        current = next;
    }

    freeMemory(list->memoryManager, list);
}

void insert(struct LinkedListCDT* list, void* data) {
    if (list == NULL)
        return;

    struct NodeCDT * newNode = (Node)allocMemory(list->memoryManager, sizeof(struct NodeCDT));
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

            freeMemory(list->memoryManager, current);
            list->size--;
            return;
        }

        previous = current;
        current = current->next;
    }
}