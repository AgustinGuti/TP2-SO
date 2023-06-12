#include <interrupts.h>
#include <linkedList.h>
#include <sharedMemory.h>

struct sharedMemSegCDT{
    char * name;
    void * initialDir;
};

typedef struct sharedMemSegCDT *sharedMemADT;
static LinkedList memList = NULL;

void *createSharedMem(char *name){
    if (memList == NULL )
    {
        memList = createLinkedList();
    }
    Iterator it = iterator(memList);
    while (hasNext(it)){
        sharedMemADT currSegment = next(it);
        if (strcmp(currSegment->name, name) == 0){
            return currSegment->initialDir;
        }
    }
    sharedMemADT newSharedMem = (sharedMemADT)malloc(sizeof(sharedMemADT));
    newSharedMem->name = malloc(strlen(name)+1);
    strcpy(newSharedMem->name, name);
    newSharedMem->initialDir = malloc(SEGMENT_SIZE);
    insert(memList, newSharedMem);

    return newSharedMem->initialDir;
}