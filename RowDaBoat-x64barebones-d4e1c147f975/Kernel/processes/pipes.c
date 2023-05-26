#include <pipes.h>

typedef struct PipeCDT{
    uint16_t *buffer;
    char *name;
    int size;
    int readIndex;
    int writeIndex;
    int readFD;
    int writeFD;
    int attached;
    sem_t sem;
}PipeCDT;

typedef struct PipesCDT{
    LinkedList pipes;
    Iterator it;
}PipesCDT;

typedef struct PipesCDT * Pipes;

static Pipes pipes = NULL;

static uint16_t currentFD = 0;


Pipe openPipe(char *name, int fds[2]){
    if(name == NULL || strlen(name) == 0){
        return NULL;
    }
    //enterCritical();
    if(pipes == NULL){
        pipes = malloc(sizeof(struct PipesCDT));
        pipes->pipes = createLinkedList();
        pipes->it = iterator(pipes->pipes);
    }
    resetIterator(pipes->it);
    while(hasNext(pipes->it)){
        Pipe pipe = next(pipes->it);
        if(strcmp(pipe->name, name) == 0){
            fds[0] = pipe->readFD;
            fds[1] = pipe->writeFD;
            pipe->attached++;
       //     leaveCritical();
            return 0;
        }
    }
    Pipe pipe = (Pipe)malloc(sizeof(PipeCDT));
    if(pipe == NULL){
        return NULL;
    }
    pipe->buffer = (char *)malloc(PIPE_SIZE*sizeof(pipe->buffer[0]));
    if(pipe->buffer == NULL){
        return NULL;
    }
    pipe->name = (char *)malloc(strlen(name) + 1);
    if(pipe->name == NULL){
        return NULL;
    }
    strcpy(pipe->name, name);
    pipe->size = PIPE_SIZE;
    pipe->readIndex = 0;
    pipe->writeIndex = 0;
    pipe->attached = 1;
    pipe->readFD = currentFD++;
    pipe->writeFD = currentFD++;
    fds[0] = pipe->readFD;
    fds[1] = pipe->writeFD;
    pipe->sem = semOpen(NULL, 0);
    if(pipe->sem == NULL){
        return NULL;
    }
    insert(pipes->pipes, pipe);
   // leaveCritical();
    return pipe;
}

int closePipe(char *name){
    if(name == NULL || strlen(name) == 0){
        return -1;
    }
  //  enterCritical();
    if(pipes == NULL){
        leaveCritical();
        return -1;
    }
    resetIterator(pipes->it);
    while(hasNext(pipes->it)){
        Pipe pipe = next(pipes->it);
        if(strcmp(pipe->name, name) == 0){
            if (pipe->attached > 1){
                pipe->attached--;
            }else{
                semClose(pipe->sem);
                free(pipe->buffer);
                free(pipe->name);
                remove(pipes->pipes, pipe);
                free(pipe);
            }
      //      leaveCritical();
            return 0;
        }
    }
  //  leaveCritical();
    return -1;
}

int readFromPipe(int fd, uint16_t *buffer, int size){
   // printf("Reading %d from pipe %d", size, fd);
    if(buffer == NULL || size < 0){
        return -1;
    }
    //enterCritical();
    if(pipes == NULL){
        //leaveCritical();
        return -1;
    }
    resetIterator(pipes->it);
    while(hasNext(pipes->it)){
        Pipe pipe = next(pipes->it);
        if(pipe->readFD == fd){
            if(pipe->readIndex == pipe->writeIndex){
                semWait(pipe->sem);
            }
            int i = 0;
            while(i < size && pipe->readIndex != pipe->writeIndex){
                buffer[i] = pipe->buffer[pipe->readIndex];
                pipe->readIndex = (pipe->readIndex + 1) % pipe->size;
                i++;
            }
          //  semPost(pipe->sem);
           // leaveCritical();
            return i;
        }
    }
   // leaveCritical();
    return -1;
}

int writeToPipe(int fd, uint16_t *buffer, int size){
    if(buffer == NULL || size < 0){
        return -1;
    }
  //  enterCritical();
    if(pipes == NULL){
       // leaveCritical();
        return -1;
    }
    resetIterator(pipes->it);
    while(hasNext(pipes->it)){
        Pipe pipe = next(pipes->it);
        if(pipe->writeFD == fd){
            int i = 0;
            while(i < size){
                if((pipe->writeIndex + 1) % pipe->size == pipe->readIndex){
                   // semWait(pipe->sem);
                }
                pipe->buffer[pipe->writeIndex] = buffer[i];
                pipe->writeIndex = (pipe->writeIndex + 1) % pipe->size;
                i++;
            }
            semPost(pipe->sem);
        //    leaveCritical();
            return i;
        }
    }
   // leaveCritical();
    return -1;
}