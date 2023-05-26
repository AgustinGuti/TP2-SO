#include <semaphores.h>
#include <interrupts.h>
#include <linkedList.h>


//Estructura de un semáforo
static struct semaphoresCDT {
    LinkedList semaphores;
    Iterator it;
} semaphoresCDT;

typedef struct semaphoreCDT {
    char *name;
    int value;
    int id;
    int waiting;
    LinkedList waiting_list;
    int attached;
}semaphoreCDT;


typedef struct semaphoresCDT * Semaphores;

static Semaphores semaphores = NULL;

static uint32_t currentSemIds = 0;

// Crea o abre un semáforo y lo identifica con el nombre name.
// Si no existe un semáforo con ese nombre, lo crea con valor value.
// Si ya existe un semáforo con ese nombre, lo abre e ignora el valor value.
sem_t semOpen(char *name, int value){
    if ((name != NULL && strlen(name) == 0) || value < 0) return NULL;

    enterCritical();
    if (semaphores == NULL){
        semaphores = malloc(sizeof(struct semaphoresCDT));
        semaphores->semaphores = createLinkedList();
        semaphores->it = iterator(semaphores->semaphores);
    }

    resetIterator(semaphores->it);
    char * newName = NULL;
    if (name != NULL){
        while (hasNext(semaphores->it)){
            sem_t sem = next(semaphores->it);
            if (strcmp(sem->name, name) == 0){
                sem->attached++;
                leaveCritical();
                return sem;
            }
        }
        newName = malloc(strlen(name) + 1);
    }

    sem_t newSem = malloc(sizeof(struct semaphoreCDT));
    newSem->name = newName;
    newSem->value = value;
    newSem->id = currentSemIds++;
    newSem->waiting = 0;
    //printf("Creating semaphore %s with id %d\n", sem->name, sem->id);
    newSem->waiting_list = createLinkedList();
    newSem->attached = 1;
    insert(semaphores->semaphores, newSem);
    leaveCritical();
    return newSem;
}

// Cierra el semáforo identificado por sem.
// Si no hay procesos esperando en el semáforo, lo elimina.
// Si hay procesos esperando en el semáforo, no lo elimina.
void semClose(sem_t sem){
    if (sem == NULL) return;
    enterCritical();
    if (sem->attached == 1){
        remove(semaphores->semaphores, sem);
        if (sem->name != NULL){
            free(sem->name);
        }
        leaveCritical();
        return;
    } else {
        sem->attached--;
    }
    leaveCritical();
    return;
}

// Decrementa en uno el valor del semáforo identificado por sem.
// Si el valor del semáforo es cero, el proceso que llama queda bloqueado.
void semWait(sem_t sem){
    if (sem == NULL) return;
    enterCritical();
    if (sem->value > 0){
        sem->value--;
        leaveCritical();
        return;
    }
    pid_t *pid = malloc(sizeof(pid_t));
    *pid = getpid();
    sem->waiting++;
    insert(sem->waiting_list, pid);
    leaveCritical();
    blockProcess(*pid);
    return;
}

// Incrementa en uno el valor del semáforo identificado por sem.
// Si hay procesos bloqueados en el semáforo, desbloquea a uno de ellos.
void semPost(sem_t sem){
    if (sem == NULL) return;
    enterCritical();
    if (sem->waiting > 0){
        sem->waiting--;
        pid_t *pid = (pid_t *)get(sem->waiting_list, 0);
        remove(sem->waiting_list, pid);
        leaveCritical();
        unblockProcess(*pid);
        free(pid);
        return;
    }
    sem->value++;
    leaveCritical();
    return;
}