// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <phylos.h>
#include <processes.h>

#define LEFT (i + philoQty - 1) % philoQty
#define RIGHT (i + 1) % philoQty

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

int philoQty = 0;
int currentMax = BLOCK;
int processToKill = -1;
int *state;
sem_t processToKillMutex;
sem_t changingQtyMutex;
sem_t mutex;
sem_t *philoSemaphores;
pid_t *philosophersPID;
int *timesEaten;
int maxTimesEaten;

char phylos(char argc, char **argv)
{
    state = (int *)malloc(BLOCK * sizeof(int));
    if (state == NULL)
    {
        printf("Error allocating memory for state\n");
        return 1;
    }
    philoSemaphores = (sem_t *)malloc(BLOCK * sizeof(sem_t));
    if (philoSemaphores == NULL)
    {
        printf("Error allocating memory for philoSemaphores\n");
        return 1;
    }
    philosophersPID = (pid_t *)malloc(BLOCK * sizeof(pid_t));
    if (philosophersPID == NULL)
    {
        printf("Error allocating memory for philosophersPID\n");
        return 1;
    }
    timesEaten = (int *)malloc(BLOCK * sizeof(int));
    if (timesEaten == NULL)
    {
        printf("Error allocating memory for timesEaten\n");
        return 1;
    }
    maxTimesEaten = 0;

    int i;
    for (i = 0; i < philoQty; i++)
    {
        state[i] = THINKING;
        philoSemaphores[i] = semOpen(NULL, 0);
    }

    if ((mutex = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore mutex\n");
        return 1;
    }

    if ((processToKillMutex = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore processToKillMutexName\n");
        return 1;
    }

    if ((changingQtyMutex = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore changingQty\n");
        return 1;
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

    for (i = 0; i < philoQty; i++)
    {
        kill(philosophersPID[i]);
    }

    free(state);
    free(philosophersPID);
    semClose(mutex);
    semClose(processToKillMutex);
    for (i = 0; i < philoQty; i++)
    {
        semClose(philoSemaphores[i]);
    }
    free(philoSemaphores);

    return 0;
}

void addPhilo()
{
    semWait(changingQtyMutex);
    philoQty++;
    if (philoQty > currentMax)
    {
        currentMax *= 2;
        state = (int *)realloc(state, currentMax * sizeof(int));
        if (state == NULL)
        {
            printf("Error reallocating memory for state\n");
            return;
        }
        philoSemaphores = (sem_t *)realloc(philoSemaphores, currentMax * sizeof(sem_t));
        if (philoSemaphores == NULL)
        {
            printf("Error reallocating memory for philoSemaphores\n");
            return;
        }
        philosophersPID = (pid_t *)realloc(philosophersPID, currentMax * sizeof(pid_t));
        if (philosophersPID == NULL)
        {
            printf("Error reallocating memory for philosophersPID\n");
            return;
        }
        timesEaten = (int *)realloc(timesEaten, currentMax * sizeof(int));
        if (timesEaten == NULL)
        {
            printf("Error reallocating memory for timesEaten\n");
            return;
        }
    }
    state[philoQty - 1] = THINKING;
    if ((philoSemaphores[philoQty - 1] = semOpen(NULL, 1)) == NULL)
    {
        printf("Error opening semaphore %d\n", philoQty - 1);
        return;
    }
    timesEaten[philoQty - 1] = maxTimesEaten;
    char foreground[2] = "0";
    char *args[4];
    args[0] = malloc(12 * sizeof(char));
    if (args[0] == NULL)
    {
        printf("Error allocating memory for args[0]\n");
        return;
    }
    strcpy(args[0], "philosopher");
    args[1] = malloc(2 * sizeof(char));
    if (args[1] == NULL)
    {
        printf("Error allocating memory for args[1]\n");
        return;
    }
    strcpy(args[1], foreground);

    args[2] = malloc(10 * sizeof(char));
    if (args[2] == NULL)
    {
        printf("Error allocating memory for args[2]\n");
        return;
    }
    args[3] = NULL;
    decToStr(args[2], philoQty - 1);
    philosophersPID[philoQty - 1] = execve(&philosopher, NULL, 0, args);
    free(args[0]);
    free(args[1]);
    free(args[2]);
    semPost(changingQtyMutex);
}

void removePhilo()
{
    semWait(changingQtyMutex);
    if (philoQty > 1)
    {
        put_forks(philoQty - 1);
        kill(philosophersPID[philoQty]);
        semClose(philoSemaphores[philoQty - 1]);
        philoQty--;
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
    semWait(philoSemaphores[i]);
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
        semPost(philoSemaphores[i]);
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

    if (philoQty > 10)
    {
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
    for (i = 0; i < philoQty; i++)
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
    return (i + philoQty - 1) % philoQty;
}

int right(int i)
{
    return (i + 1) % philoQty;
}