#include <semaphores.h>
#include <interrupts.h>

//Estructura de un semáforo
struct semaphore {
    char *name;
    int value;
    int id;
    int waiting;
    pid_t *waiting_list;
};

sem_t semaphores = NULL;

static uint32_t currentSemIds = 0;

// Crea o abre un semáforo y lo identifica con el nombre name.
// Si no existe un semáforo con ese nombre, lo crea con valor value.
// Si ya existe un semáforo con ese nombre, lo abre e ignora el valor value.
sem_t semOpen(char *name, int value){
    if (semaphores == NULL){
        semaphores = createLinkedList();
    }
    if (name == NULL || strlen(name) == 0 || value < 0) return NULL;

    IteratorPtr it = iterator(semaphores);
    sem_t sem = next(it);
    int count = 0;
    int16_t size = getSize(semaphores);
    while (count < size){
        if (strcmp(sem->name, name) == 0){
            return sem;
        }
        sem = next(it);
        count++;
    }
    freeIterator(it);
    sem = malloc(sizeof(struct semaphore));
    sem->name = malloc(strlen(name) + 1);
    strcpy(sem->name, name);
    sem->value = value;
    sem->id = currentSemIds++;
    sem->waiting = 0;
    sem->waiting_list = createLinkedList();
    insert(semaphores, sem);
    return sem;
}

// Cierra el semáforo identificado por sem.
// Si no hay procesos esperando en el semáforo, lo elimina.
// Si hay procesos esperando en el semáforo, no lo elimina.
void semClose(sem_t sem){
    if (sem == NULL) return;
    _cli();
    if (sem->waiting == 0){
        sem_t it = iterator(semaphores);
        int count = 0;
        int16_t size = getSize(semaphores);
        while (count < size){
            if (it->id == sem->id){
                remove(semaphores, it);
                return;
            }
            it = next(it);
            count++;
        }
    }
    _sti();
    return;
}

// Decrementa en uno el valor del semáforo identificado por sem.
// Si el valor del semáforo es cero, el proceso que llama queda bloqueado.
void semWait(sem_t sem){
    if (sem == NULL) return;
    _cli();
    if (sem->value > 0){
        sem->value--;
        return;
    }
    sem->waiting++;
    pid_t *pid = malloc(sizeof(pid_t));
    *pid = getpid();
    insert(sem->waiting_list, pid);
    blockProcess(*pid);
    _sti();
    return;
}

// Incrementa en uno el valor del semáforo identificado por sem.
// Si hay procesos bloqueados en el semáforo, desbloquea a uno de ellos.
void semPost(sem_t sem){
    if (sem == NULL) return;
    if (sem->waiting > 0){
        sem->waiting--;
        pid_t *pid = (pid_t *)get(sem->waiting_list, 0);
        remove(sem->waiting_list, pid);
        unblockProcess(*pid);
        return;
    }
    sem->value++;
    return;
}