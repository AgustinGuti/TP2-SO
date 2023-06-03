// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <semaphores.h>
#include <interrupts.h>
#include <linkedList.h>

// struct of semaphores
static struct semaphoresCDT
{
    LinkedList semaphoresList;
    Iterator it;
} semaphoresCDT;

typedef struct semaphoreCDT
{
    char *name;
    int value;
    LinkedList waitingList;
    LinkedList connectedProcesses;
    Iterator itConnectedProcesses;
} semaphoreCDT;

typedef struct semaphoresCDT *Semaphores;

static Semaphores semaphores = NULL;

// Creates or opens a semaphore and identifies it with the name "name".
// If the name is NULL, create an anonymous semaphore
// If a semaphore with that name does not exist, create it with value "value".
// If a semaphore with that name already exists, open it and ignore the value "value".
sem_t semOpen(char *name, int value)
{
    if ((name != NULL && strlen(name) == 0) || value < 0)
        return NULL;

    enterCritical();
    if (semaphores == NULL && name != NULL)
    {
        semaphores = malloc(sizeof(struct semaphoresCDT));
        if (semaphores == NULL)
        {
            leaveCritical();
            return NULL;
        }
        semaphores->semaphoresList = createLinkedList();
        semaphores->it = iterator(semaphores->semaphoresList);
    }
    char *newName = NULL;
    if (name != NULL)
    {
        resetIterator(semaphores->it);
        while (hasNext(semaphores->it))
        {
            sem_t sem = next(semaphores->it);
            if (strcmp(sem->name, name) == 0)
            {
                resetIterator(sem->itConnectedProcesses);
                while (hasNext(sem->itConnectedProcesses))
                {
                    Process proc = next(sem->itConnectedProcesses);
                    if (proc == NULL)
                        continue;
                    if (proc->pid == getpid())
                    {
                        return sem;
                    }
                }
                pid_t *pid = malloc(sizeof(pid_t));
                if (pid == NULL)
                {
                    leaveCritical();
                    return NULL;
                }
                *pid = getpid();
                insert(sem->connectedProcesses, pid);
                leaveCritical();
                return sem;
            }
        }
        newName = malloc(strlen(name) + 1);
        if (newName == NULL)
        {
            leaveCritical();
            return NULL;
        }
        strcpy(newName, name);
    }

    sem_t newSem = malloc(sizeof(struct semaphoreCDT));
    if (newSem == NULL)
    {
        leaveCritical();
        free(newName);
        newName = NULL;
        return NULL;
    }
    newSem->name = newName;
    newSem->value = value;
    newSem->waitingList = createLinkedList();
    newSem->connectedProcesses = createLinkedList();
    newSem->itConnectedProcesses = iterator(newSem->connectedProcesses);
    pid_t *pid = malloc(sizeof(pid_t));
    if (pid == NULL)
    {
        leaveCritical();
        free(newName);
        newName = NULL;
        free(newSem);
        newSem = NULL;
        return NULL;
    }
    *pid = getpid();
    insert(newSem->connectedProcesses, pid);

    if (name != NULL)
    {
        insert(semaphores->semaphoresList, newSem);
    }
    leaveCritical();
    return newSem;
}

// Closes the semaphore identified by sem.
// If there are no processes waiting on the semaphore, kill it.
// If there are processes waiting on the semaphore, don't kill it.
void semClose(sem_t sem)
{
    if (sem == NULL)
        return;
    enterCritical();
    if (getSize(sem->connectedProcesses) == 1)
    {
        resetIterator(sem->itConnectedProcesses);
        while (hasNext(sem->itConnectedProcesses))
        {
            pid_t *pid = next(sem->itConnectedProcesses);
            remove(sem->connectedProcesses, pid);
            free(pid);
            pid = NULL;
        }
        Iterator it = iterator(sem->waitingList);
        while (hasNext(it))
        {
            pid_t *pid = next(it);
            remove(sem->waitingList, pid);
            free(pid);
            pid = NULL;
        }
        freeIterator(it);
        freeIterator(sem->itConnectedProcesses);
        destroyLinkedList(sem->connectedProcesses);
        destroyLinkedList(sem->waitingList);

        if (sem->name != NULL)
        {
            remove(semaphores->semaphoresList, sem);
            free(sem->name);
            if (getSize(semaphores->semaphoresList) == 0)
            {
                printf("Deleteing list\n");
                freeIterator(semaphores->it);
                destroyLinkedList(semaphores->semaphoresList);
                free(semaphores);
                semaphores = NULL;
            }
        }
        free(sem);
        sem = NULL;

        leaveCritical();
        return;
    }
    else
    {
        resetIterator(sem->itConnectedProcesses);
        while (hasNext(sem->itConnectedProcesses))
        {
            pid_t *pid = next(sem->itConnectedProcesses);
            if (*pid == getpid())
            {
                remove(sem->connectedProcesses, pid);
                free(pid);
                pid = NULL;
            }
        }
    }
    leaveCritical();
    return;
}

// Decrease the value of the semaphore identified by sem by one.
// If the value of the semaphore is zero, the calling process is blocked.
void semWait(sem_t sem)
{
    if (sem == NULL)
        return;
    enterCritical();
    if (sem->value > 0)
    {
        sem->value--;
        leaveCritical();
        return;
    }
    pid_t *pid = malloc(sizeof(pid_t));
    if (pid == NULL)
    {
        leaveCritical();
        return;
    }
    *pid = getpid();
    insert(sem->waitingList, pid);

    leaveCritical();
    blockProcess(*pid);
    return;
}

// Increases the value of the semaphore identified by sem by one.
// If there are blocked processes in the semaphore, unblock one of them.
void semPost(sem_t sem)
{
    if (sem == NULL)
        return;
    enterCritical();
    if (getSize(sem->waitingList) > 0)
    {
        pid_t *pid = (pid_t *)get(sem->waitingList, 0);
        remove(sem->waitingList, pid);
        leaveCritical();
        unblockProcess(*pid);
        free(pid);
        return;
    }
    sem->value++;
    leaveCritical();
    return;
}
