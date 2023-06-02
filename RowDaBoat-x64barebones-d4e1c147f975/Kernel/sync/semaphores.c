// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <semaphores.h>
#include <interrupts.h>
#include <linkedList.h>

// Estructura de un semáforo
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

// Crea o abre un semáforo y lo identifica con el nombre name.
// Si name es NULL, crea un semáforo anónimo
// Si no existe un semáforo con ese nombre, lo crea con valor value.
// Si ya existe un semáforo con ese nombre, lo abre e ignora el valor value.
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
        return NULL;
    }
    newSem->name = newName;
    newSem->value = value;
    // printf("Creating semaphore %s with id %d\n", sem->name, sem->id);
    newSem->waitingList = createLinkedList();
    newSem->connectedProcesses = createLinkedList();
    newSem->itConnectedProcesses = iterator(newSem->connectedProcesses);
    pid_t *pid = malloc(sizeof(pid_t));
    if (pid == NULL)
    {
        leaveCritical();
        free(newName);
        free(newSem);
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

// Cierra el semáforo identificado por sem.
// Si no hay procesos esperando en el semáforo, lo elimina.
// Si hay procesos esperando en el semáforo, no lo elimina.
void semClose(sem_t sem)
{
    if (sem == NULL)
        return;
    enterCritical();
    if (getSize(sem->connectedProcesses) == 1)
    {
        destroyLinkedList(sem->waitingList);
        resetIterator(sem->itConnectedProcesses);
        while (hasNext(sem->itConnectedProcesses))
        {
            pid_t *pid = next(sem->itConnectedProcesses);
            remove(sem->connectedProcesses, pid);
            free(pid);
        }
        Iterator it = iterator(sem->waitingList);
        while(hasNext(it)){
            pid_t *pid = next(it);
            remove(sem->waitingList, pid);
            free(pid);            
        }
        freeIterator(it);
        freeIterator(sem->itConnectedProcesses);
        destroyLinkedList(sem->connectedProcesses);
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
            }
        }
    }
    leaveCritical();
    return;
}

// Decrementa en uno el valor del semáforo identificado por sem.
// Si el valor del semáforo es cero, el proceso que llama queda bloqueado.
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

// Incrementa en uno el valor del semáforo identificado por sem.
// Si hay procesos bloqueados en el semáforo, desbloquea a uno de ellos.
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
