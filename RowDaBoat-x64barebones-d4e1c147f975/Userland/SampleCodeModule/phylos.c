#include <phylos.h>


#define N 5
#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N];
sem_t mutex;
sem_t s[N];
pid_t philosophersPID[N];

void philosopher(char argc, char **argv);
void take_forks(int i);
void put_forks(int i);
void test(int i);
void eat(int phil);
void think(int phil);
void printState();

void phylos(){
    int i;
    for(i = 0; i < N; i++){
        state[i] = THINKING;
        char name[2]= "0";
        decToStr(name, i);
        s[i]= semOpen(name, 0);
    }
    char *mutexName = "mutex";
    mutex =semOpen(mutexName, 1);

    //int execve(void* entryPoint, char * const argv[]);
    //create the processes for each philosopher
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
            // addPhilo();
            printf("add\n");
            break;
        case 'r':
            printf("remove\n");
            // removePhilo();
            break;
        case 'q':
            printf("quit\n");
            exit = 1;
            break;
        }
        
    }
        

    return;
}

void philosopher(char argc, char **argv){
    int i = strToNum(argv[0], 1);
    int j;

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
    test(LEFT);
    test(RIGHT);
    semPost(mutex);
}

void test(int i){
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
        state[i] = EATING;
        semPost(s[i]);
    }
}

void eat(int phil)
{
    //long for so it takes a while
    int i=0;
    while(i<100000000){
        i++;
    }
	printState();
}

void think(int phil)
{
    //long for so it takes a while
    int i=0;
    while(i<100000000){
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