// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <testSync.h>
#include <stdint.h>
#include <stdio.h>
#include "sysCallInterface.h"
#include "test_util.h"
#include "stddef.h"
#include <processes.h>
#include <semaphores.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 10

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc)
{
  uint64_t aux = *p;
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[])
{
  uint64_t n;
  int8_t inc;
  int8_t use_sem;
  sem_t sem;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
  {
    sem = semOpen(SEM_ID, 1);
    if (!sem)
    {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }
  }

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem)
      semWait(sem);
    slowInc(&global, inc);
    if (use_sem)
      semPost(sem);
  }

  if (use_sem)
    semClose(sem);

  return 0;
}

char test_sync(char argc, char *argv[])
{ //{n, use_sem}
  int pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
  {
    return 1;
  }

  char *argvDec[] = {"my_process_inc", "0", argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {"my_process_inc", "0", argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pids[i] = execve(&my_process_inc, NULL, 0, argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = execve(&my_process_inc, NULL, 0, argvInc);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    waitpid(pids[i]);
    waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  if (global != 0)
  {
    printf("Race condition detectada\n");
    printf("Valor final: %d\n", global);
  }
  else
  {
    printf("Test completado exitosamente\n");
  }

  return 0;
}