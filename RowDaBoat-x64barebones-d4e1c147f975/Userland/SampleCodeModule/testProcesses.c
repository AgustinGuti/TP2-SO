// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <testProcesses.h>
#include <stdio.h>
#include <memory.h>
#include <sysCallInterface.h>

#include <stdio.h>
#include "test_util.h"

enum State
{
    RUNNING,
    BLOCKED,
    KILLED
};

typedef struct P_rq
{
    int32_t pid;
    enum State state;
} p_rq;

char testProcesses(char argc, char *argv[])
{
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;
    uint64_t max_processes;
    char *argvAux[] = {"endless_loop", "0", NULL};

    if (argc != 1)
        return 1;

    if ((max_processes = satoi(argv[0])) <= 0)
        return 1;

    p_rq p_rqs[max_processes];

    int count = 0;
    while (1)
    {
        // Create max_processes processes
        for (rq = 0; rq < max_processes; rq++)
        {
            p_rqs[rq].pid = execve(&endless_loop, NULL, 0, argvAux);
            if (p_rqs[rq].pid == -1)
            {
                printf("test_processes: ERROR creating process\n");
                return -1;
            }
            else
            {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0)
        {
            for (rq = 0; rq < max_processes; rq++)
            {
                action = GetUniform(100) % 2;
                switch (action)
                {
                case 0:
                    if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
                    {
                        if (kill(p_rqs[rq].pid) == -1)
                        {
                            printf("test_processes: ERROR killing process\n");
                            return -1;
                        }
                        p_rqs[rq].state = KILLED;
                        alive--;
                    }
                    break;

                case 1:
                    if (p_rqs[rq].state == RUNNING)
                    {
                        if (blockProcess(p_rqs[rq].pid) == -1)
                        {
                            printf("test_processes: ERROR blocking process\n");
                            return -1;
                        }
                        p_rqs[rq].state = BLOCKED;
                    }
                    break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < max_processes; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
                {
                    if (blockProcess(p_rqs[rq].pid) == -1)
                    {
                        printf("test_processes: ERROR unblocking process\n");
                        return -1;
                    }
                    p_rqs[rq].state = RUNNING;
                }
        }
        printf("Test %d completado exitosamente\n", count++);
    }
    return 0;
}


void readerProcess(){
    char * sharedMem = _sys_createSharedMem("sharedMem");
    char * msg = malloc(100);
    memcpy(msg, sharedMem, 11);
    printf("Mensaje recibido: %s\n", msg);
}

void writerProcess(){
    char * sharedMem = _sys_createSharedMem("sharedMem");
    char *text = malloc(strlen("Hola Mundo") + 1);
    strcpy(text, "Hola Mundo");
    text[10]=0;
    memcpy(sharedMem, text, strlen(text) + 1);
}