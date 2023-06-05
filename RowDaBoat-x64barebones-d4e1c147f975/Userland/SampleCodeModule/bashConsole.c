// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <bashConsole.h>

#define EOF -1

#define MAX_ARGS 10
#define MAX_ARG_LENGTH 100
#define MAX_COMMAND_NAME_LENGTH 50

char parseAndExecuteCommands(uint8_t *str, int length);
void getCommandAndArgs(char *str, char *args[], int *argQty, char *command, int length);
char getCommandIndex(char *commandName);
int getFullCommand(uint8_t *str, int length, char **args);

char help(char argc, char **argv);
char setFontSize(char argc, char **argv);
char exitConsole(char argumentQty, char **arguments);
char callMalloc(char argc, char **argv);
char callFree(char argc, char **argv);
char callGetMemoryStatus(char argc, char **argv);
char callBlock(char argc, char **argv);
char callKill(char argc, char **argv);
char callNice(char argc, char **argv);
char callSleep(char argc, char **argv);
char callRealloc(char argc, char **argv);
char callTestSync(char argc, char **argv);
char callTestNoSync(char argc, char **argv);

typedef struct commandCDT
{
    char *name;
    char (*function)(char argc, char **argv);
    char argMaxQty;
    char argMinQty;
    char *description;
    char executable;
    char test;
} commandCDT;

#define COMMAND_QTY 29

static commandCDT commands[COMMAND_QTY] = {
    {"help", help, 1, 0, "Imprime en pantalla los comandos disponibles. Si el argumento identifica a otro comando, explica su funcionamiento.", 1, 0},
    {"clear", cleanScreen, 0, 0, "Vacia la consola.", 1, 0},
    {"memory-dump", memoryDump, 1, 1, "Recibe como parametro una direccion de memoria e imprime los 32 bytes de memoria posteriores a la misma.", 1, 0},
    {"time", displayTime, 0, 0, "Imprime en pantalla la hora del sistema.", 1, 0},
    {"zero-division", zeroDivision, 0, 0, "Genera la excepcion zero division y muestra en pantalla los registros en el momento del error.", 1, 0},
    {"invalid-opcode", invalidOpcode, 0, 0, "Genera la excepcion invalid opcode y muestra en pantalla los registros en el momento del error.", 1, 0},
    {"set-font-size", setFontSize, 1, 1, "Permite agrandar o achicar la dimension de del texto en pantalla por argumento.", 1, 0},
    {"inforeg", printRegs, 0, 0, "Imprime el valor de los ultimos registros guardados. Para guardar los registros se debe presionar la tecla LCTRL.", 1, 0},
    {"exit", exitConsole, 0, 0, "Termina la ejecucion de la consola.", 0, 0},
    {"malloc", callMalloc, 1, 1, "Reserva una cantidad de memoria dada por parametro.", 1, 0},
    {"free", callFree, 1, 1, "Libera la memoria reservada en la direccion dada por parametro.", 1, 0},
    {"ps", printProcesses, 1, 0, "Imprime en pantalla los procesos en ejecucion. El argumento '-k' muestra los procesos eliminados", 1, 0},
    {"mem", callGetMemoryStatus, 0, 0, "Imprime en pantalla el estado de la memoria.", 1, 0},
    {"block", callBlock, 1, 1, "Bloquea un proceso dado por parametro.", 1, 0},
    {"kill", callKill, 1, 1, "Elimina un proceso dado por parametro.", 1, 0},
    {"nice", callNice, 2, 2, "Modifica la prioridad de un proceso dado por parametro.", 1, 0},
    {"phylo", phylos, 0, 0, "Ejecuta el problema de los filosofos comensales.", 1, 0},
    {"cat", cat, 0, 0, "Imprime en pantalla el contenido de un archivo dado por parametro.", 1, 0},
    {"sleep", callSleep, 1, 1, "Duerme un proceso dado por parametro.", 1, 0},
    {"wc", wc, 0, 0, "Imprime en pantalla la cantidad de lineas de su input.", 1, 0},
    {"realloc", callRealloc, 2, 2, "Reasigna la memoria de un puntero dado por parametro.", 1, 0},
    {"loop", loop, 1, 1, "Imprime su ID con un saludo cada una determinada cantidad de segundos.", 1, 0},
    {"filter", filter, 0, 0, "Imprime en pantalla las vocales de su input.", 1, 0},
    {"test-mm", testMM, 1, 1, "Ejecuta el test de memoria.", 1, 1},
    {"test-sync", callTestSync, 1, 1, "Ejecuta el test de sincronizacion.", 1, 1},
    {"test-no-sync", callTestNoSync, 1, 1, "Ejecuta el test sin un mecanismo de sincronizacion.", 1, 1},
    {"test-processes", testProcesses, 1, 1, "Ejecuta el test de procesos.", 1, 1},
    {"test-prio", testPrio, 0, 0, "Ejecuta el test de prioridades.", 1, 1},
    {"set-auto-prio", setAutoPrio, 1, 1, "Activa o desactiva el cambio automatico de prioridad. Recibe 1 para activarlo y 0 para descativarlo", 1, 0},
};

int startConsole()
{
    char printBuf[1024];         // This is used to print to screen in real time
    uint8_t commandBuffer[1024]; // This is used to prepare the next command
    uint16_t commandBufferPos = 0;
    char exit = 0; // Create console command for exit

    printText(LINE_INDICATOR);
    while (exit == 0)
    {
        int readQty = scanf("%1s", 1, printBuf);
        if (readQty > 0)
        {
            for (int i = 0; i < readQty; i++)
            {
                if ((commandBufferPos > 0) || printBuf[i] != BACKSPACE)
                {
                    putChar(printBuf[i]);

                    if (printBuf[i] == NEWLINE)
                    {
                        exit = parseAndExecuteCommands(commandBuffer, commandBufferPos);
                        if(!exit){
                            printf("\n%s", LINE_INDICATOR);
                        }
                        for (int arg_idx = 0; arg_idx < commandBufferPos; arg_idx++)
                        { // Clear buffer
                            commandBuffer[arg_idx] = 0;
                        }
                        commandBufferPos = 0;
                    }
                    else if (printBuf[i] == BACKSPACE)
                    {
                        commandBufferPos--;
                    }
                    else
                    {
                        commandBuffer[commandBufferPos++] = printBuf[i]; // Stores in command buffer
                    }
                }
            }
        }
    }
    return 1;
}

void freeArray(char **array, int length)
{

    for (int i = 0; i < length; i++)
    {
        if (array[i] != NULL)
        {
            free(array[i]);
        }
    }
    free(array);
}

void **mallocArray(int length)
{
    void **array = malloc(sizeof(void *) * length);
    if (array == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < length; i++)
    {
        array[i] = malloc(MAX_ARG_LENGTH + 1);
        if (array[i] == NULL)
        {
            freeArray((char **)array, i);
            return NULL;
        }
    }
    return array;
}

char parseAndExecuteCommands(uint8_t *str, int length)
{
    char pipePos = -1;
    for (int i = 0; i < length; i++)
    {
        if (str[i] == '|')
        {
            pipePos = i;
            break;
        }
    }
    if (pipePos == -1)
    {
        // No pipe, execute a single command
        char **argv = (char **)mallocArray(MAX_ARGS + 2);
        int command = getFullCommand(str, length, argv);
        if (command == -1)
        {
            freeArray(argv, MAX_ARGS + 2);
            return 0;
        }
        if (commands[command].executable == 0)
        {
            int res = commands[command].function(0, argv);
            freeArray(argv, MAX_ARGS + 2);
            return res;
        }

        execve(commands[command].function, NULL, 0, argv);

        freeArray(argv, MAX_ARGS + 2);
        return 0;
    }

    Pipe connectingPipe = pipe(NULL, NULL);
    Pipe pipes1[2] = {NULL, connectingPipe};
    Pipe pipes2[2] = {connectingPipe, NULL};
    int pipeQty = 2;

    char **argv1 = (char **)mallocArray(MAX_ARGS + 2);
    if (argv1 == NULL)
    {
        return 0;
    }
    char **argv2 = (char **)mallocArray(MAX_ARGS + 2);
    if (argv2 == NULL)
    {
        freeArray(argv1, MAX_ARGS + 2);
        return 0;
    }

    int command1 = getFullCommand(str, pipePos, argv1);
    int command2 = getFullCommand(str + pipePos + 1, length - pipePos - 1, argv2);
    if (command1 == -1 || command2 == -1)
    {
        freeArray(argv1, MAX_ARGS + 2);
        freeArray(argv2, MAX_ARGS + 2);
        return 0;
    }
    if (commands[command1].executable == 0 || commands[command2].executable == 0)
    {
        freeArray(argv1, MAX_ARGS + 2);
        freeArray(argv2, MAX_ARGS + 2);
        printText("Error: one of the commands is not executable.\n");
        return 0;
    }

    strcpy(argv1[1], "0"); // El primer proceso no puede estar en foreground
    execve(commands[command1].function, pipes1, pipeQty, argv1);
    execve(commands[command2].function, pipes2, pipeQty, argv2);

    freeArray(argv1, MAX_ARGS + 2);
    freeArray(argv2, MAX_ARGS + 2);

    return 0;
}

int getFullCommand(uint8_t *str, int length, char **args)
{
    char command[length + 1];
    char **arguments = (char **)mallocArray(MAX_ARGS + 2);
    if (arguments == NULL)
    {
        return -1;
    }
    int argc = 0;
    char commandName[length + 1];
    memcpy(command, str, length);
    command[length] = 0;
    getCommandAndArgs(command, arguments, &argc, commandName, length + 1);
    int commandIndex = getCommandIndex(commandName);
    if (commandIndex == -1)
    {
        freeArray(arguments, MAX_ARGS + 2);
        return -1;
    }
    memcpy(args[0], commandName, length + 1);
    if (commands[commandIndex].executable)
    {
        int hasBackground = 0;
        if (argc > 0)
        {
            if (strcmp(arguments[argc - 1], "&") == 0)
            {
                hasBackground = 1;
                strcpy(args[1], "0");
            }
            else
            {
                strcpy(args[1], "1");
            }
        }
        else
        {
            strcpy(args[1], "1");
        }
        if (argc - hasBackground > commands[commandIndex].argMaxQty || argc + hasBackground < commands[commandIndex].argMinQty)
        {
            printf("Cantidad de argumentos invalida para %s\n", commandName);
            freeArray(arguments, MAX_ARGS + 2);
            return -1;
        }

        for (int i = 0; i < argc - hasBackground; i++)
        {
            strcpy(args[i + 2], arguments[i]);
        }
        free(args[argc + 2 - hasBackground]);
        args[argc + 2 - hasBackground] = NULL;
    }
    else
    {
        if (argc > commands[commandIndex].argMaxQty || argc < commands[commandIndex].argMinQty)
        {
            printf("Cantidad de argumentos invalida para %s\n", commandName);
            freeArray(arguments, MAX_ARGS + 2);
            return -1;
        }
    }

    freeArray(arguments, MAX_ARGS + 2);
    return commandIndex;
}

char getCommandIndex(char *commandName)
{
    int commandIndex;
    for (commandIndex = 0; commandIndex < COMMAND_QTY; commandIndex++)
    {
        if (strcmp(commandName, commands[commandIndex].name) == 0)
        {
            break;
        }
    }
    if (commandIndex == COMMAND_QTY)
    {
        printf("Comando no encontrado. Utilize 'help' para ver los comandos disponibles\n");
        return -1;
    }
    return commandIndex;
}

void getCommandAndArgs(char *str, char *args[], int *argQty, char *command, int length)
{
    int i = 0;
    while (str[i] == ' ')
    {
        i++;
    }
    int offset = i;
    int cmd_idx = 0;
    while (str[i] != ' ' && str[i] != 0)
    {
        i++;
        cmd_idx++;
    }
    memcpy(command, str + offset, cmd_idx);
    command[cmd_idx] = 0;
    *argQty = 0;
    int argLen = 0;
    while (str[i] != 0)
    {
        if (str[i] == ' ')
        {
            if (argLen > 0)
            {
                args[*argQty][argLen] = 0;
                (*argQty)++;
                argLen = 0;
            }
        }
        else
        {
            args[*argQty][argLen++] = str[i];
        }
        i++;
    }

    args[*argQty][argLen] = 0;
    if (argLen > 0)
    {
        (*argQty)++;
    }
}

char help(char argc, char **argv)
{
    if (argc == 0)
    {
        printf("Los comandos disponibles son:\n");
        for (int i = 0; i < COMMAND_QTY; i++)
        {
            printf("%s\n", commands[i].name);
        }
    }
    else if (argc > 1)
    {
        printf("Demasiados argumentos para help\n");
    }
    else
    {
        if (strcmp(argv[0], "please") == 0)
        {
            printf("No.\n");
            return 0;
        }
        else if (strcmp(argv[0], "tests") == 0)
        {
            for (int i = 0; i < COMMAND_QTY; i++)
            {
                if (commands[i].test)
                {
                    printf("%s: %s\n", commands[i].name, commands[i].description);
                }
            }
            return 0;
        }
        else if (strcmp(argv[0], "all") == 0)
        {
            for (int i = 0; i < COMMAND_QTY; i++)
            {
                printf("%s: %s\n", commands[i].name, commands[i].description);
            }
            return 0;
        }
        else
        {
            for (int i = 0; i < COMMAND_QTY; i++)
            {
                if (strcmp(argv[0], commands[i].name) == 0)
                {
                    printf("%s\n", commands[i].description);
                    return 0;
                }
            }
        }
        printf("Argumento invalido para help\n");
    }
    return 0;
}

char callFree(char argc, char **argv)
{
    if (argc == 1 && isHexaNumber(argv[0]))
    {
        char flag = 0;
        uint64_t *ptr = (uint64_t *)hexaStrToNum(argv[0], strlen(argv[0]), &flag);
        if (flag == 1)
        {
            printf("Numero muy grande. Overflow\n");
        }
        else
        {
            uint64_t freedBytes = free(ptr);
            printf("%x bytes liberados\n", freedBytes);
        }
    }
    else
    {
        printf("Argumento invalido para free\n");
    }
    return 0;
}

char callMalloc(char argc, char **argv)
{
    if (argc == 1 && isHexaNumber(argv[0]))
    {
        char flag = 0;
        uint64_t size = hexaStrToNum(argv[0], strlen(argv[0]), &flag);
        if (flag == 1)
        {
            printf("Numero muy grande. Overflow\n");
        }
        else
        {
            uint64_t *ptr = malloc(size);
            if (ptr == NULL)
            {
                printf("No se reservo memoria\n");
            }
            else
            {
                printf("Se reservo memoria en la direccion %x\n", ptr);
            }
        }
    }
    else
    {
        printf("Argumento invalido para malloc\n");
    }
    return 0;
}

char callRealloc(char argc, char **argv)
{
    if (argc != 2 || !isHexaNumber(argv[0]) || !isHexaNumber(argv[1]))
    {
        printf("Argumento invalido para realloc\n");
        return 0;
    }
    char flag1 = 0, flag2 = 0;
    uint64_t *ptr = (uint64_t *)hexaStrToNum(argv[0], strlen(argv[0]), &flag1);

    uint64_t newSize = (uint64_t)hexaStrToNum(argv[1], strlen(argv[1]), &flag2);
    if (flag1 == 1 || flag2 == 1)
    {
        printf("Argumento inválido\n");
        return 0;
    }
    uint64_t *newPtr = realloc(ptr, newSize);
    if (newPtr == NULL)
    {
        printf("No se reservo memoria\n");
    }
    else
    {
        printf("Se reservo memoria en la direccion %x\n", newPtr);
    }
    return 0;
}

char callKill(char argc, char **argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para kill\n");
    }
    else
    {
        int pid = strToNum(argv[0], strlen(argv[0]));
        kill(pid);
    }
    return 0;
}

char callGetMemoryStatus(char argc, char **argv)
{
    if (argc != 0)
    {
        printf("Argumento invalido para get-memory-status\n");
    }
    else
    {
        uint64_t *memStatus = getMemoryStatus();
        if (memStatus == NULL)
        {
            printf("No se pudo obtener el estado de la memoria\n");
            return 1;
        }
        printf("Memoria total: %x\n", memStatus[0]);
        printf("Memoria reservada: %x\n", memStatus[1]);
        printf("Memoria libre: %x\n", memStatus[2]);
    }
    return 0;
}

char callBlock(char argc, char **argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para block. Debe recibir el PID del proceso a bloquear.\n");
    }
    else
    {
        int num = strToNum(argv[0], strlen(argv[0]));
        blockProcess(num);
    }
    return 0;
}

char callNice(char argc, char **argv)
{
    if (argc != 2)
    {
        printf("Argumento invalido para nice\n");
    }
    else
    {
        int pid = strToNum(argv[0], strlen(argv[0]));
        int priority = strToNum(argv[1], strlen(argv[1]));
        int returnValue = _sys_nice(pid, priority);
        if (returnValue < 0)
        {
            printf("Error al cambiar la prioridad\n");
        }
        else
        {
            printf("Prioridad del proceso con pid %d cambiada a %d\n", pid, returnValue);
        }
    }
    return 0;
}

char setFontSize(char argc, char **argv)
{
    if (argc < 1)
    {
        printf("Ingrese una fuente entre 1 y 4\n");
    }
    else if (argc > 1)
    {
        printf("Demasiados argumentos\n");
    }
    else
    {
        int num = strToNum(argv[0], strlen(argv[0]));
        if (num > 4 || num < 1)
        {
            printf("Ingrese una fuente entre 1 y 4\n");
        }
        else
        {
            _sys_setFontSize(num, 1);
        }
    }
    return 0;
}

char exitConsole(char argumentQty, char **arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para exit-console\n");
    }
    else
    {
        return 1;
    }
    return 0;
}

char callSleep(char argc, char **argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para sleep\n");
    }
    else
    {
        int num = strToNum(argv[0], strlen(argv[0]));
        sleep(num);
    }
    return 0;
}

char callTestSync(char argc, char **argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para test-sync\n");
    }
    else
    {
        char *args[] = {argv[0], "1"};
        test_sync(argc + 1, args);
    }
    return 0;
}

char callTestNoSync(char argc, char **argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para test-no-sync\n");
    }
    else
    {
        char *args[] = {argv[0], "0"};
        test_sync(argc + 1, args);
    }
    return 0;
}