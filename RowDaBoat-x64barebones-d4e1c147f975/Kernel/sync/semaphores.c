#include <semaphores.h>
#include <interrupts.h>
#include <linkedList.h>

//Estructura de un semáforo
static struct semaphoresCDT {
    sem_t *semaphores;
} semaphoresCDT;

typedef struct semaphoresCDT * Semaphores;

static Semaphores semaphores = NULL;

static uint32_t currentSemIds = 0;

// Crea o abre un semáforo y lo identifica con el nombre name.
// Si no existe un semáforo con ese nombre, lo crea con valor value.
// Si ya existe un semáforo con ese nombre, lo abre e ignora el valor value.
sem_t semOpen(char *name, int value){
    if (name == NULL || strlen(name) == 0 || value < 0) return NULL;
    if (semaphores == NULL){
        semaphores = malloc(sizeof(struct semaphoresCDT));
        semaphores->semaphores = createLinkedList();
    }

    IteratorPtr it = iterator(semaphores->semaphores);
    sem_t sem;
    while (hasNext(it)){
        sem = next(it);
        if (strcmp(sem->name, name) == 0){
            freeIterator(it);
            printf("Already exists semaphore %s with value %d\n", sem->name, sem->value);
            return sem;
        }
    }
    freeIterator(it);
    sem = malloc(sizeof(struct semaphore));
    sem->name = malloc(strlen(name) + 1);
    strcpy(sem->name, name);
    sem->value = value;
    sem->id = currentSemIds++;
    sem->waiting = 0;
    //printf("Creating semaphore %s with id %d\n", sem->name, sem->id);
    sem->waiting_list = createLinkedList();
    insert(semaphores->semaphores, sem);
    return sem;
}

// Cierra el semáforo identificado por sem.
// Si no hay procesos esperando en el semáforo, lo elimina.
// Si hay procesos esperando en el semáforo, no lo elimina.
void semClose(sem_t sem){
    if (sem == NULL) return;
    if (sem->waiting == 0){
        IteratorPtr it = iterator(semaphores->semaphores);
        while (hasNext(it)){
            sem_t currSem = next(it);
            if (currSem->id == sem->id){
                remove(semaphores->semaphores, currSem);
                return;
            }
        }
    }
    return;
}

// Decrementa en uno el valor del semáforo identificado por sem.
// Si el valor del semáforo es cero, el proceso que llama queda bloqueado.
void semWait(sem_t sem){
    if (sem == NULL) return;
   // printf("----------------------------------------------------------------------ENTERING WAIT\n");
    enterCritical();
    if (sem->value > 0){
        sem->value--;
   //     printf("---------------------------------EXITING WAIT\n");
        leaveCritical();
        return;
    }
   // printf("Blocking %d\n", getpid());
    pid_t *pid = malloc(sizeof(pid_t));
    *pid = getpid();
    sem->waiting++;
    //printf("Waiting %d\n", sem->waiting);
    insert(sem->waiting_list, pid);
    //printf("Blocking %d\n", *pid);
    blockProcess(*pid);
    free(pid);
   // printf("-------------------------------EXITING WAIT\n");
   // triggerTimer();
    leaveCritical();
    return;
}

// Incrementa en uno el valor del semáforo identificado por sem.
// Si hay procesos bloqueados en el semáforo, desbloquea a uno de ellos.
void semPost(sem_t sem){
    if (sem == NULL) return;
    enterCritical();
   // printf("---------------------------------------------------------------------ENTERING POST\n");
    if (sem->waiting > 0){
        sem->waiting--;
        pid_t *pid = (pid_t *)get(sem->waiting_list, 0);
     //   printf("Unblocking %d\n", *pid);
        remove(sem->waiting_list, pid);
        unblockProcess(*pid);
     //   printf("---------------------------------EXITING POST\n");
        leaveCritical();
       // triggerTimer();
        return;
    }
    sem->value++;
    leaveCritical();
  //  printf("--------------------------------EXITING POST\n");
    return;
}