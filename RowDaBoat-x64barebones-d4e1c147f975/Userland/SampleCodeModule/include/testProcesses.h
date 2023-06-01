#include <stdio.h>
#include <sysCallInterface.h>
#include <semaphores.h>
#include <stddef.h>
#include <processes.h>
#include <pipes.h>

int processA();

void processB();

void processC();

void cat();

void wc();

void loop(int sec);

void filter();