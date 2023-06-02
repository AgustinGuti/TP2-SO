#include <scheduler.h>
#include <functions.h>
#include <linkedList.h>

typedef struct semaphoreCDT *sem_t;

// Crea o abre un semáforo y lo identifica con el nombre name.
// Si no existe un semáforo con ese nombre, lo crea con valor value.
// Si ya existe un semáforo con ese nombre, lo abre e ignora el valor value.
sem_t semOpen(char *name, int value);

// Cierra el semáforo identificado por sem.
// Si no hay procesos esperando en el semáforo, lo elimina.
// Si hay procesos esperando en el semáforo, no lo elimina.
void semClose(sem_t sem);

// Decrementa en uno el valor del semáforo identificado por sem.
// Si el valor del semáforo es cero, el proceso que llama queda bloqueado.
void semWait(sem_t sem);

// Incrementa en uno el valor del semáforo identificado por sem.
// Si hay procesos bloqueados en el semáforo, desbloquea a uno de ellos.
void semPost(sem_t sem);