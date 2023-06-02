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
    if (semaphores == NULL)
    {
        semaphores = malloc(sizeof(struct semaphoresCDT));
        semaphores->semaphoresList = createLinkedList();
        semaphores->it = iterator(semaphores->semaphoresList);
    }
    resetIterator(semaphores->it);
    char *newName = NULL;
    if (name != NULL)
    {
        while (hasNext(semaphores->it))
        {
            sem_t sem = next(semaphores->it);
            if (strcmp(sem->name, name) == 0)
            {
                resetIterator(sem->itConnectedProcesses);
                while (hasNext(sem->itConnectedProcesses))
                {
                    Process proc = next(sem->itConnectedProcesses);
                    if (proc->pid == getpid())
                    {
                        return sem;
                    }
                }
                pid_t *pid = malloc(sizeof(pid_t));
                *pid = getpid();
                insert(sem->connectedProcesses, pid);
                leaveCritical();
                return sem;
            }
        }
        newName = malloc(strlen(name) + 1);
        strcpy(newName, name);
    }

    sem_t newSem = malloc(sizeof(struct semaphoreCDT));
    newSem->name = newName;
    newSem->value = value;
    // printf("Creating semaphore %s with id %d\n", sem->name, sem->id);
    newSem->waitingList = createLinkedList();
    newSem->connectedProcesses = createLinkedList();
    newSem->itConnectedProcesses = iterator(newSem->connectedProcesses);
    pid_t *pid = malloc(sizeof(pid_t));
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
        if (sem->name != NULL)
        {
            free(sem->name);
        }
        destroyLinkedList(sem->waitingList);
        freeIterator(sem->itConnectedProcesses);
        destroyLinkedList(sem->connectedProcesses);
        remove(semaphores->semaphoresList, sem);
        leaveCritical();
        return;
    }
    else
    {
        resetIterator(sem->itConnectedProcesses);
        while (hasNext(sem->itConnectedProcesses))
        {
            Process proc = next(sem->itConnectedProcesses);
            if (proc->pid == getpid())
            {
                remove(sem->connectedProcesses, proc);
                free(proc);
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

