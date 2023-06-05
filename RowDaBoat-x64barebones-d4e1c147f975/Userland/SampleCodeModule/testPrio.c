// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <testPrio.h>
#include <stdint.h>
#include <stdio.h>
#include <sysCallInterface.h>
#include <processes.h>
#include "test_util.h"
#include <functions.h>

#define MINOR_WAIT 50000000 // TODO: Change this value to prevent a process from flooding the screen

#define WAIT 1000000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

char testPrio(char argc, char *args[])
{
  if (argc > 0)
  {
    printf("test_prio: demasiados argumentos\n");
    return 1;
  }
  int64_t pids[TOTAL_PROCESSES];
  char waitStr[10] = {0};
  decToStr(waitStr, MINOR_WAIT); 
  char *argv[] = {"endless_loop_print", "0", waitStr, NULL};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = execve(&endless_loop_print, NULL, 0, argv);

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");


  for (i = 0; i < TOTAL_PROCESSES; i++)
    _sys_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");


  for (i = 0; i < TOTAL_PROCESSES; i++)
    blockProcess(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    _sys_nice(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    blockProcess(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    kill(pids[i]);

  printf("Test completado exitosamente\n");
  return 0;
}
