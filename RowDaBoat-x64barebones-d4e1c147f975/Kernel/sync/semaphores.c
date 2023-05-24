#include <semaphores.h>
#include <interrupts.h>
#include <linkedList.h>


//Estructura de un semáforo
static struct semaphoresCDT {
    LinkedList semaphores;
    IteratorPtr it;
} semaphoresCDT;

typedef struct semaphoreCDT {
    char *name;
    int value;
    int id;
    int waiting;
    LinkedList waiting_list;
}semaphoreCDT;


typedef struct semaphoresCDT * Semaphores;

static Semaphores semaphores = NULL;

static uint32_t currentSemIds = 0;

// Crea o abre un semáforo y lo identifica con el nombre name.
// Si no existe un semáforo con ese nombre, lo crea con valor value.
// Si ya existe un semáforo con ese nombre, lo abre e ignora el valor value.
sem_t semOpen(char *name, int value){
    if (name == NULL || strlen(name) == 0 || value < 0) return NULL;

    enterCritical();
    if (semaphores == NULL){
        printf("Creating semaphores\n");
        semaphores = malloc(sizeof(struct semaphoresCDT));
        semaphores->semaphores = createLinkedList();
        semaphores->it = iterator(semaphores->semaphores);
    }

    resetIterator(semaphores->it, semaphores->semaphores);
    sem_t sem;
    printf("Searching for semaphore %s\n", name);
    while (hasNext(semaphores->it)){
        sem = next(semaphores->it);
        printf("Checking semaphore %s\n", sem->name);
        if (strcmp(sem->name, name) == 0){
            printf("Already exists semaphore %s with value %d\n", sem->name, sem->value);
            leaveCritical();
            return sem;
        }
    }

    sem_t newSem = malloc(sizeof(struct semaphoreCDT));
    newSem->name = malloc(strlen(name) + 1);
    strcpy(newSem->name, name);
    newSem->value = value;
    newSem->id = currentSemIds++;
    newSem->waiting = 0;
    //printf("Creating semaphore %s with id %d\n", sem->name, sem->id);
    newSem->waiting_list = createLinkedList();
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
    if (sem->waiting == 0){
        resetIterator(semaphores->it, semaphores->semaphores);
        while (hasNext(semaphores->it)){
            sem_t currSem = next(semaphores->it);
            if (currSem->id == sem->id){
                remove(semaphores->semaphores, currSem);
                free(currSem->name);
                leaveCritical();
                return;
            }
        }
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
   // blockProcess(*pid);
    leaveCritical();
   // triggerTimer();
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
     //   unblockProcess(*pid);
       // free(pid);
        leaveCritical();
        return;
    }
    sem->value++;
    leaveCritical();
    return;
}