#include <pipes.h>

typedef struct PipeCDT{
    char *buffer;
    char *name;
    int size;
    int readIndex;
    int writeIndex;
    int attached;
    sem_t sem;
}PipeCDT;

typedef struct PipesCDT{
    LinkedList pipes;
    Iterator it;
}PipesCDT;

typedef struct PipesCDT * Pipes;

static Pipes pipes = NULL;

Pipe openPipe(char *name){
    if(name != NULL && strlen(name) == 0){
        return NULL;
    }
    if(pipes == NULL){
        pipes = malloc(sizeof(struct PipesCDT));
        pipes->pipes = createLinkedList();
        pipes->it = iterator(pipes->pipes);
    }
    char *newName = NULL;
    if (name != NULL){
        resetIterator(pipes->it);
        while(hasNext(pipes->it)){
            Pipe pipe = next(pipes->it);
            if(strcmp(pipe->name, name) == 0){
                pipe->attached++;
                return 0;
            }
        }
        newName = (char *)malloc(strlen(name) + 1);
        if(newName == NULL){
            return NULL;
        }
        strcpy(newName, name);
    }

    Pipe pipe = (Pipe)malloc(sizeof(PipeCDT));
    if(pipe == NULL){
        return NULL;
    }
    pipe->buffer = (char *)malloc(PIPE_SIZE*sizeof(pipe->buffer[0]));
    if(pipe->buffer == NULL){
        return NULL;
    }
    pipe->name = newName;
    pipe->size = PIPE_SIZE;
    pipe->readIndex = 0;
    pipe->writeIndex = 0;
    pipe->attached = 1;
    pipe->sem = semOpen(NULL, 0);
    if(pipe->sem == NULL){
        return NULL;
    }
    if (newName != NULL){
        insert(pipes->pipes, pipe);
    }
    return pipe;
}

int closePipe(Pipe pipe){
    if(pipe == NULL){
        return -1;
    }
    if (pipe->attached > 1){
        printf("Pipe %s is still attached\n", pipe->name);
        pipe->attached--;
    }else{
        semClose(pipe->sem);
        free(pipe->buffer);
        if (pipe->name != NULL){
            free(pipe->name);
        }
        free(pipe);
    }
    return 0;
}

int readFromPipe(Pipe pipe, uint16_t *buffer, int size){
    if(pipe == NULL || buffer == NULL || size < 0){
        return -1;
    }
    if(pipe->readIndex == pipe->writeIndex){
        semWait(pipe->sem);
    }
    int i = 0;
    while(i < size && pipe->readIndex != pipe->writeIndex){
        buffer[i] = pipe->buffer[pipe->readIndex];
        pipe->readIndex = (pipe->readIndex + 1) % pipe->size;
        i++;
    }
    return i;
}

int writeToPipe(Pipe pipe, uint16_t *buffer, int size){
    if(pipe == NULL || buffer == NULL || size < 0){
        return -1;
    }
    int i = 0;
    while(i < size){
        pipe->buffer[pipe->writeIndex] = buffer[i];
        pipe->writeIndex = (pipe->writeIndex + 1) % pipe->size;
        i++;
    }
    semPost(pipe->sem);
    return i;
}
