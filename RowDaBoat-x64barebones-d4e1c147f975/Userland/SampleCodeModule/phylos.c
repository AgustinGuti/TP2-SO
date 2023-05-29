#include <phylos.h>

#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2


int N = 5;
int *state;
sem_t mutex;
sem_t *s;
pid_t *philosophersPID;

void philosopher(char argc, char **argv);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void eat(int phil);
void think(int phil);
void printState();
void addPhilo();
void removePhilo();
int left(int i);
int right(int i);

void phylos(){
    state = (int *)malloc(N*sizeof(int));
    s = (sem_t *)malloc(N*sizeof(sem_t));
    philosophersPID = (pid_t *)malloc(N*sizeof(pid_t));

    int i;
    for(i = 0; i < N; i++){
        state[i] = THINKING;
        char name[2]= "0";
        decToStr(name, i);
        s[i]= semOpen(name, 0);
    }
    char *mutexName = "mutex";
    mutex =semOpen(mutexName, 1);

    char foreground[2] = "0";
    char *args[4];

    args[0] = "philosopher";
    args[1] = foreground;
    args[3]= NULL;
    args[2] = (char *)malloc(2*sizeof(char));
    for(i = 0; i < N; i++){
        decToStr(args[2], i);    
        philosophersPID[i] = execve(&philosopher, args);
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
            printf("quit\n");
            exit = 1;
            break;
        }
        
    }
        

    return;
}

void addPhilo(){
    N++;
    state = (int *)realloc(state, N*sizeof(int));
    s = (sem_t *)realloc(s, N*sizeof(sem_t));
    philosophersPID = (pid_t *)realloc(philosophersPID, N*sizeof(pid_t));
    state[N-1] = THINKING;
    char name[2]= "0";
    decToStr(name, N-1);
    s[N-1]= semOpen(name, 0);
    char foreground[2] = "0";
    char *args[4];
    args[0] = "philosopher";
    args[1] = foreground;
    args[3]= NULL;
    args[2] = (char *)malloc(2*sizeof(char));
    decToStr(args[2], N-1);
    philosophersPID[N-1] = execve(&philosopher, args);
}

void removePhilo(){
    if(N>1){
        N--;
        kill(philosophersPID[N]);
        state = (int *)realloc(state, N*sizeof(int));
        s = (sem_t *)realloc(s, N*sizeof(sem_t));
        philosophersPID = (pid_t *)realloc(philosophersPID, N*sizeof(pid_t));
    }
    else{
        printf("Can't remove more philosophers\n");
    }
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
    test(i);
    semPost(mutex);
    semWait(s[i]);
}

void put_forks(int i){
    semWait(mutex);
    state[i] = THINKING;
    test(left(i));
    test(right(i));
    semPost(mutex);
}

void test(int i){
    if(state[i] == HUNGRY && state[left(i)] != EATING && state[right(i)] != EATING){
        state[i] = EATING;
        semPost(s[i]);
    }
}

void eat(int phil)
{
    //long for so it takes a while
    int i=0;
    while(i<5000000){
        i++;
    }
	printState();
}

void think(int phil)
{
    //long for so it takes a while
    int i=0;
    while(i<5000000){
        i++;
    }
}

void printState(){
    int i;
    for(i = 0; i < N; i++){
        switch (state[i])
        {
        case THINKING:
            printf(". ");
            break;
        case HUNGRY:
            printf(". ");
            break;
        case EATING:
            printf("E ");
            break;
        }
    }
    printf("\n");
}

int left(int i){
    return (i + N - 1) % N;
}

int right(int i){
    return (i + 1) % N;
}