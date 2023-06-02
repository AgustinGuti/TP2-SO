#include <linkedList.h>

#define MAX_FREE_NODES 100

// Iterator structure
typedef struct NodeCTD
{
    void *data;
    struct NodeCTD *prev;
    struct NodeCTD *next;
} NodeCTD;
typedef struct LinkedListCDT
{
    NodeCTD *head;
    NodeCTD *tail;
    uint32_t size;
    NodeCTD **free;
    uint16_t freeNodes;
} LinkedListCDT;
typedef struct IteratorCDT
{
    LinkedList list;
    NodeCTD *current;
} IteratorCDT;


// Linked list structure

// Function to create an empty linked list
LinkedList createLinkedList()
{
    LinkedList list = (LinkedList)malloc(sizeof(LinkedListCDT));
    if (list == NULL){
        printerr("Error: malloc failed in insert\n");
        return;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->freeNodes = 0;
    list->free = malloc(MAX_FREE_NODES * sizeof(Node));
    return list;
}

// Function to destroy a linked list and free the memory
void destroyLinkedList(LinkedList list)
{
    NodeCTD *current = list->head;
    while (current != NULL)
    {
        NodeCTD *next = current->next;
        free(current);
        current = next;
    }
    while(list->freeNodes > 0){
        int idx = 0;
        while(list->free[idx] == NULL){
            idx++;
        }
        free(list->free[idx]);
        list->free[idx] = NULL;
        list->freeNodes--;
    }
    free(list->free);
    free(list);
}

// Function to insert an element at the end of the linked list
void insert(LinkedList list, void *data)
{
    Node newNode = NULL;
    if (list->freeNodes){
        int idx = 0;
        while(list->free[idx] == NULL){
            idx++;
        }
        newNode = list->free[idx];
        list->free[idx] = NULL;
        list->freeNodes--;
    }else{
        newNode = (NodeCTD *)malloc(sizeof(NodeCTD));
    }
    if (newNode == NULL){
        printerr("Error: malloc failed in insert\n");
        return;
    }
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
void remove(LinkedList list, void *data)
{
    NodeCTD *current = list->head;
    while (current != NULL)
    {
        if (current->data == data)
        {
            if (current == list->head)
                list->head = current->next;

            if (current == list->tail)
                list->tail = current->prev;

            if (current->prev != NULL)
                current->prev->next = current->next;

            if (current->next != NULL)
                current->next->prev = current->prev;

            if (list->freeNodes < MAX_FREE_NODES){
                int idx = 0;
                while (list->free[idx] != NULL){
                    idx++;
                }                
                list->free[idx] = current;
                list->freeNodes++;
            }else{
                free(current);
            }
            list->size--;
            return;
        }
        current = current->next;
    }
}

void *removeFirst(LinkedList list){
    NodeCTD *current = list->head;
    if (current == NULL)
        return NULL;

    if (current == list->head)
        list->head = current->next;

    if (current == list->tail)
        list->tail = current->prev;

    if (current->prev != NULL)
        current->prev->next = current->next;

    if (current->next != NULL)
        current->next->prev = current->prev;

    list->size--;
    return current->data;
}

void switchList(LinkedList origin, LinkedList dest){
    if (origin == NULL || dest == NULL)
        return;
    NodeCTD *current = origin->head;
    if (current == NULL)
        return;

    if (current == origin->head)
        origin->head = current->next;

    if (current == origin->tail)
        origin->tail = current->prev;

    if (current->prev != NULL)
        current->prev->next = current->next;

    if (current->next != NULL)
        current->next->prev = current->prev;

    if (dest->tail != NULL)
        dest->tail->next = current;
    dest->tail = current;

    if (dest->head == NULL)
        dest->head = current;

    dest->size++;
    origin->size--;
}

// Function to get an element at a specific index
void *get(LinkedList list, int index)
{
    if (index < 0 || index >= list->size)
        return NULL;

    NodeCTD *current = list->head;
    for (int i = 0; i < index; i++)
        current = current->next;

    return current->data;
}

void printList(LinkedList list)
{
    printf("Size of list: %d\n", list->size);
    printf("printList\n");
    Node current = list->head;
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
Iterator iterator(LinkedList list)
{
    Iterator iterator = (Iterator)malloc(sizeof(IteratorCDT));
    if (iterator == NULL){
        printerr("Error: malloc failed in insert\n");
        return NULL;
    }
    iterator->list = list;
    iterator->current = list->head;
    return iterator;
}

void resetIterator(Iterator iterator)
{
    iterator->current = iterator->list->head;
}

// Function to check if there are more elements in the iterator
int hasNext(Iterator iterator)
{
    return (iterator->current != NULL);
}

// Function to get the next element from the iterator
void *next(Iterator iterator)
{
    if (!hasNext(iterator))
        return NULL;

    void *data = iterator->current->data;
    iterator->current = iterator->current->next;
    return data;
}

// Function to get the size of the linked list
int getSize(LinkedList list)
{
    return list->size;
}

// Function to free the memory of an iterator
void freeIterator(Iterator iterator)
{
    free(iterator);
}

void moveToBack(LinkedList list, void *data)
{
    NodeCTD *current = list->head;
    while (current != NULL)
    {
        if (current->data == data)
        {
            if (current == list->head && current == list->tail)
                return;

            if (current == list->head)
                list->head = current->next;

            if (current == list->tail)
                list->tail = current->prev;

            if (current->prev != NULL)
                current->prev->next = current->next;

            if (current->next != NULL)
                current->next->prev = current->prev;

            current->prev = list->tail;
            current->next = NULL;
            list->tail->next = current;
            list->tail = current;
            return;
        }
        current = current->next;
    }
}

// Function to move the head of the linked list to the back
void headToBack(LinkedList list)
{
    if (list->head != NULL && list->head != list->tail)
    {
        NodeCTD *newTail = list->head;
        list->head = list->head->next;
        list->head->prev = NULL;
        list->tail->next = newTail;
        newTail->prev = list->tail;
        newTail->next = NULL;
        list->tail = newTail;
    }
}

void * findItem(LinkedList list, void *data, int (*comparator)(void *, void *)){
    NodeCTD *current = list->head;
    while (current != NULL)
    {
        if (comparator(current->data, data) == 0)
        {
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}
