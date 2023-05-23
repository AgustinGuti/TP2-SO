#include <linkedList.h>

// Node structure
typedef struct NodeCTD {
    void* data;
    struct NodeCTD* prev;
    struct NodeCTD* next;
} NodeCTD;

// Linked list structure
typedef struct LinkedListCDT {
    NodeCTD* head;
    NodeCTD* tail;
    int size;
} LinkedListCDT;

// Iterator structure
typedef struct IteratorCDT {
    NodeCTD* current;
} IteratorCDT;


// Function to create an empty linked list
LinkedList createLinkedList() {
    LinkedList list = (LinkedList)malloc(sizeof(LinkedListCDT));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

// Function to destroy a linked list and free the memory
void destroyLinkedList(LinkedList list) {
    NodeCTD* current = list->head;
    while (current != NULL) {
        NodeCTD* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

// Function to insert an element at the end of the linked list
void insert(LinkedList list, void* data) {
    NodeCTD* newNode = (NodeCTD*)malloc(sizeof(NodeCTD));
    newNode->data = data;
    newNode->prev = list->tail;
    newNode->next = NULL;
    if (list->tail != NULL)
        list->tail->next = newNode;

    list->tail = newNode;

    if (list->head == NULL)
        list->head = newNode;


    list->size++;
}

// Function to remove an element from the linked list
void remove(LinkedList list, void* data) {
    NodeCTD* current = list->head;
    while (current != NULL) {
        if (current->data == data) {
            if (current == list->head)
                list->head = current->next;

            if (current == list->tail)
                list->tail = current->prev;

            if (current->prev != NULL)
                current->prev->next = current->next;

            if (current->next != NULL)
                current->next->prev = current->prev;

            free(current);
            list->size--;
            return;
        }
        current = current->next;
    }
}

// Function to get an element at a specific index
void* get(LinkedList list, int index) {
    if (index < 0 || index >= list->size)
        return NULL;

    NodeCTD* current = list->head;
    for (int i = 0; i < index; i++)
        current = current->next;

    return current->data;
}

void printList(LinkedList list){
    printf("Size of list: %d\n", list->size);
    printf("printList\n");
    Node current = list->head;
    printf("list->head: %x\n", list->head);
    while (current != NULL)
    {
        printf("Current: %x\n", current);
        printf("Current->data: %x\n", current->data);
        printf("Current->next: %x\n", current->next);
        current = current->next;
    }
    
    return;
}

// Function to create an iterator for the linked list
IteratorPtr iterator(LinkedList list) {
    IteratorPtr iterator = (IteratorPtr)malloc(sizeof(IteratorCDT));
    iterator->current = list->head;
    return iterator;
}

void resetIterator(IteratorPtr iterator, LinkedList list) {
    iterator->current = list->head;
}

// Function to check if there are more elements in the iterator
int hasNext(IteratorPtr iterator) {
    return (iterator->current != NULL);
}

// Function to get the next element from the iterator
void* next(IteratorPtr iterator) {
    if (!hasNext(iterator))
        return NULL;

    void* data = iterator->current->data;
    iterator->current = iterator->current->next;
    return data;
}

// Function to get the size of the linked list
int getSize(LinkedList list) {
    return list->size;
}

// Function to free the memory of an iterator
void freeIterator(IteratorPtr iterator) {
    free(iterator);
}

// Function to move the head of the linked list to the back
void headToBack(LinkedList list) {
    if (list->head != NULL && list->head != list->tail) {
        NodeCTD* newTail = list->head;
        list->head = list->head->next;
        list->head->prev = NULL;
        list->tail->next = newTail;
        newTail->prev = list->tail;
        newTail->next = NULL;
        list->tail = newTail;
    }
}
