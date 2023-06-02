#include <phylos.h>
#include <processes.h>

#define LEFT (i + N - 1) % N
#define RIGHT (i + 1) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define BLOCK 100

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

int N = 0;
int currentMax = BLOCK;
int processToKill = -1;
int *state;
sem_t processToKillMutex;
sem_t changingQtyMutex;
sem_t mutex;
sem_t *s;
pid_t *philosophersPID;
int *timesEaten;
int maxTimesEaten;

void phylos()
{
    state = (int *)malloc(BLOCK * sizeof(int));
    s = (sem_t *)malloc(BLOCK * sizeof(sem_t));
    philosophersPID = (pid_t *)malloc(BLOCK * sizeof(pid_t));
    timesEaten = (int *)malloc(BLOCK * sizeof(int));
    maxTimesEaten = 0;

    int i;
    for (i = 0; i < N; i++)
    {
        state[i] = THINKING;
        s[i] = semOpen(NULL, 0);
    }

    if ((mutex = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore mutex\n");
        return;
    }

    if ((processToKillMutex = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore processToKillMutexName\n");
        return;
    }

    if ((changingQtyMutex = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore changingQty\n");
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        addPhilo();
    }

    int exit = 0;
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

    for(i = 0; i < N; i++){
        kill(philosophersPID[i]);
    }   

    free(state);
    free(philosophersPID);
    semClose(mutex);
    semClose(processToKillMutex);
    for(i = 0; i < N; i++){
        semClose(s[i]);
    }
    free(s);


    return;
}

void addPhilo()
{
    semWait(changingQtyMutex);
    N++;
    if (N > currentMax)
    {
        currentMax *= 2;
        state = (int *)realloc(state, currentMax * sizeof(int));
        s = (sem_t *)realloc(s, currentMax * sizeof(sem_t));
        philosophersPID = (pid_t *)realloc(philosophersPID, currentMax * sizeof(pid_t));
        timesEaten = (int *)realloc(timesEaten, currentMax * sizeof(int));
    }
    state[N - 1] = THINKING;
    if ((s[N - 1] = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore %d\n", N - 1);
        return;
    }
    timesEaten[N - 1] = maxTimesEaten;
    char foreground[2] = "0";
    char *args[4];
    args[0] = malloc(12 * sizeof(char));
    strcpy(args[0], "philosopher");
    args[1] = malloc(2 * sizeof(char));
    strcpy(args[1], foreground);

    args[2] = malloc(10 * sizeof(char));
    args[3] = NULL;
    decToStr(args[2], N - 1);
    philosophersPID[N - 1] = execve(&philosopher, NULL, 0, args);
    semPost(changingQtyMutex);
}

void removePhilo()
{
    semWait(changingQtyMutex);
    if (N > 1)
    {
        put_forks(N - 1);
        kill(philosophersPID[N]);
        semClose(s[N - 1]);
        N--;
    }
    else
    {
        printf("Can't remove more philosophers\n");
    }
    semPost(changingQtyMutex);
}

void philosopher(char argc, char **argv)
{
    int i = strToNum(argv[0], strlen(argv[0]));
    while (1)
    {
        think(i);
        take_forks(i);
        eat(i);
        put_forks(i);
    }
}

void take_forks(int i)
{
    semWait(mutex);
    state[i] = HUNGRY;
    test(i);
    semPost(mutex);
    semWait(s[i]);
}

void put_forks(int i)
{
    semWait(mutex);
    state[i] = THINKING;
    test(left(i));
    test(right(i));
    semPost(mutex);
}

void test(int i)
{
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING && timesEaten[i] <= timesEaten[LEFT] + 2 && timesEaten[i] <= timesEaten[RIGHT] + 2)
    {
        state[i] = EATING;
        timesEaten[i]++;
        if (timesEaten[i] > maxTimesEaten)
        {
            maxTimesEaten = timesEaten[i];
        }
        semPost(s[i]);
    }
}

void eat(int phil)
{
    // long for so it takes a while
    int i = 0;
    while (i < 1000000)
    {
        i++;
    }
    printState();

    if (N > 10){
     //   printProcesses(0, NULL);
    }
}

void think(int phil)
{
    // long for so it takes a while
    int i = 0;
    while (i < 1000000)
    {
        i++;
    }
}

void printState()
{
    semWait(changingQtyMutex);
    int i;
    for (i = 0; i < N; i++)
    {
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

int left(int i)
{
    return (i + N - 1) % N;
}

int right(int i)
{
    return (i + 1) % N;
}