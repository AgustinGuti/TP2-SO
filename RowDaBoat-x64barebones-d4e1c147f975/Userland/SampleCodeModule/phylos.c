#include <phylos.h>

#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define BLOCK 100

int N = 0;
int currentMax = BLOCK;
int *state;
int processToKill=-1;
sem_t mutex;
sem_t *forks;
sem_t processToKillMutex;
pid_t *philosophersPID;
sem_t changingQtyMutex;

void philosopher(char argc, char **argv);
void take_forks(int i);
void put_forks(int i);
void eat(int phil);
void think(int phil);
void printState();
void addPhilo();
void removePhilo();
int left(int i);
int right(int i);

void phylos(){
    state = (int *)malloc(currentMax*sizeof(int));
    forks= (sem_t *)malloc(currentMax*sizeof(sem_t));
    philosophersPID = (pid_t *)malloc(currentMax*sizeof(pid_t));

    int i;
    for(i = 0; i < N; i++){
        state[i] = THINKING;
        char name[2]= "0";
        decToStr(name, i);
        if((forks[i]= semOpen(name, 1)) == NULL){
            printf("Error opening semaphore %s\n", name);
            return;
        }
       
    }
    
    if((mutex = semOpen(NULL, 1)) == NULL){
        printf("Error opening semaphore mutex\n");
        return;
    }
        
    if((processToKillMutex = semOpen(NULL, 1)) == NULL){
        printf("Error opening semaphore processToKillMutexName\n");
        return;
    }

    if((changingQtyMutex = semOpen(NULL, 1)) == NULL){
        printf("Error opening semaphore changingQty\n");
        return;
    }


    for (int i = 0; i < 5; i++){
        addPhilo();
    }

    int exit=0;
    while (!exit)
    {
        char c = getChar();
        switch (c)
        {
        case 'a':
            addPhilo();
            break;
        case 'r':
            removePhilo();
            break;
        case 'q':
            exit = 1;
            break;
        }
        
    }
    //Kill childs
    for(i = 0; i < N; i++){
        kill(philosophersPID[i]);
    }   

    free(state);
    free(forks);
    free(philosophersPID);
    semClose(mutex);
    semClose(processToKillMutex);
    for(i = 0; i < N; i++){
        char name[2]= "0";
        decToStr(name, i);
        semClose(forks[i]);
    }

    return;
}

void addPhilo(){
    semWait(changingQtyMutex);
    N++;
    if(N>currentMax){
        currentMax *=2;
        state = (int *)realloc(state, currentMax*sizeof(int));
        forks = (sem_t *)realloc(forks, currentMax*sizeof(sem_t));
        philosophersPID = (pid_t *)realloc(philosophersPID, currentMax*sizeof(pid_t));
    }
    state[N-1] = THINKING;
    if((forks[N-1] = semOpen(NULL, 1)) == NULL){
        printf("Error opening semaphore %d\n", N-1);
        return;
    }
    char foreground[2] = "0";
    char *args[4];

    args[0] = malloc(12*sizeof(char));
    strcpy(args[0], "philosopher");
    args[1] = malloc(2*sizeof(char));
    strcpy(args[1], foreground);
    args[2] = malloc(3*sizeof(char));
    args[3]= NULL;
    decToStr(args[2], N-1);
    philosophersPID[N-1] = execve(&philosopher,NULL,0, args);
    semPost(changingQtyMutex);
}

void removePhilo(){
    semWait(changingQtyMutex);
    if (N > 2){
        put_forks(N-1);
        kill(philosophersPID[N-1]);
        semClose(forks[N-1]);
        N--;
        semPost(changingQtyMutex);
    }else{
        printf("Can't remove more philosophers\n");
    }
    semPost(changingQtyMutex);

}

void philosopher(char argc, char **argv){
    int i = strToNum(argv[0], 1);
    while(1){
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}

void take_forks(int i){
    semWait(mutex);
    state[i] = HUNGRY;
    semPost(mutex);
    if(i%2==0){
        semWait(forks[i]);
        semWait(forks[right(i)]);
        semWait(mutex);
        state[i] = EATING;
        semPost(mutex);
    }
    else{
        semWait(forks[right(i)]);
        semWait(forks[i]);
        semWait(mutex);
        state[i] = EATING;
        semPost(mutex);
    }
}

void put_forks(int i){
    semPost(forks[i]);
    semPost(forks[right(i)]);
    semWait(mutex);
    state[i] = THINKING;
    semPost(mutex);
}


void eat(int phil)
{
    //long for so it takes a while
    int i=0;
    while(i<10000000){
        i++;
    }
	printState();
}

void think(int phil)
{
    //long for so it takes a while
    int i=0;
    while(i<10000000){
        i++;
    }
}

void printState(){
    semWait(changingQtyMutex);
    int i;
    for(i = 0; i < N; i++){
        switch (state[i])
        {
        case THINKING:
            printf(". ");
            break;
        case HUNGRY:
            printf("- ");
            break;
        case EATING:
            printf("E ");
            break;
        }
    }
    printf("\n");
    
    semPost(changingQtyMutex);
}

int left(int i){
    return (i + 1) % N;
}

int right(int i){
    return (i - 1 + N) % N;
}