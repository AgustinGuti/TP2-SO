#include <bashConsole.h>
#include <stddef.h>
#include <processes.h>
#include <testProcesses.h>
#include <tests.h>
#include <memory.h>
#include <phylos.h>

#define EOF -1

#define MAX_ARGS 10
#define MAX_ARG_LENGTH 100
#define MAX_COMMAND_NAME_LENGTH 50

extern void zeroDivision();
extern void displayTime();
char parseAndExecuteCommands(uint8_t *str, int length);
void getCommandAndArgs(char *str, char *args[], int *argQty, char *command, int length);
char getCommandIndex(char *commandName);
int getFullCommand(char *str, int length, int *argc,char **args);

char help(char argc, char **argv);
char setFontSize(char argc, const char** argv);
char exitConsole(uint8_t argumentQty, const char** arguments);
char callMalloc(char argc, const char** argv);
char callFree(char argc, const char** argv);
char callGetMemoryStatus(char argc, const char** argv);
char callBlock(char argc, const char** argv);
char callKill(char argc, const char** argv);
char callNice(char argc, const char** argv);
char callTestMM(char argc, const char** argv);
char callSleep(char argc, const char **argv);


typedef struct command {
    char *name;
    char (*function)(char argc, char **argv);
    char argMaxQty;
    char argMinQty;
    char *description;
    char executable;
} command;

#define COMMAND_QTY 24

static command commands[COMMAND_QTY] = {
    {"help", &help, 1, 0, "Imprime en pantalla los comandos disponibles. Si el argumento identifica a otro comando, explica su funcionamiento.", 1},
    {"clear", &cleanScreen, 0, 0, "Vacia la consola.", 1},
    {"tron", &tron, 0, 0, "Ejecuta el juego \"Tron Light Cycles\" para dos jugadores.", 1},
    {"memory-dump", &memoryDump, 1, 1, "Recibe como parametro una direccion de memoria e imprime los 32 bytes de memoria posteriores a la misma.", 1},
    {"time", &displayTime, 0, 0, "Imprime en pantalla la hora del sistema.", 1},
    {"zero-division", &zeroDivision, 0, 0, "Genera la excepcion zero division y muestra en pantalla los registros en el momento del error.", 1},
    {"invalid-opcode", &invalidOpcode, 0, 0, "Genera la excepcion invalid opcode y muestra en pantalla los registros en el momento del error.", 1},
    {"set-font-size", &setFontSize, 1, 1, "Permite agrandar o achicar la dimension de del texto en pantalla por argumento.", 1},
    {"inforeg", &printRegs, 0, 0, "Imprime el valor de los ultimos registros guardados. Para guardar los registros se debe presionar la tecla LCTRL.", 1},
    {"exit", &exitConsole, 0, 0, "Termina la ejecucion de la consola.", 0},
    {"himno-alegria", &himnoAlegria, 0, 0, "Reproduce el himno de la alegria.", 1},
    {"malloc", &callMalloc, 1, 1, "Reserva una cantidad de memoria dada por parametro.", 1},
    {"free", &callFree, 1, 1, "Libera la memoria reservada en la direccion dada por parametro.", 1},
    {"ps", &printProcesses, 0, 0, "Imprime en pantalla los procesos en ejecucion.", 1},
    {"mem", &callGetMemoryStatus, 0, 0, "Imprime en pantalla el estado de la memoria.", 1},
    {"block", &callBlock, 1, 1, "Bloquea un proceso dado por parametro.", 1},
    {"kill", &callKill, 1, 1, "Elimina un proceso dado por parametro.", 1},
    {"nice", &callNice, 2, 2, "Modifica la prioridad de un proceso dado por parametro.", 1},
    {"test-mm", &callTestMM, 1, 1, "Ejecuta el test de memoria.", 1},
    {"phylo", &phylos, 0, 0, "Ejecuta el problema de los filosofos comensales.", 1},
    {"cat", &cat, 0, 0, "Imprime en pantalla el contenido de un archivo dado por parametro.", 1},
    {"test-sync", &test_sync, 2, 2, "Ejecuta el test de sincronizacion.", 1},
    {"sleep", &callSleep, 1, 1, "Duerme un proceso dado por parametro.", 1},
    {"wc", &wc, 0, 0, "Imprime en pantalla la cantidad de lineas de su input.", 1}

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
                if ((commandBufferPos > 0) || (commandBufferPos == 0 && printBuf[i] != BACKSPACE))
                {
                    putChar(printBuf[i]);

                    if (printBuf[i] == NEWLINE)
                    {
                        exit = parseAndExecuteCommands(commandBuffer, commandBufferPos);
                        printText(LINE_INDICATOR);
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

char parseAndExecuteCommands(uint8_t *str, int length)
{
    char pipePos = -1;
    for (int i = 0; i < length; i++)
    {
        if (str[i] == '_')
        {
            pipePos = i;
            break;
        }
    }
    if (pipePos == -1)
    {
        // No pipe, execute a single command
        char **argv = malloc(sizeof(char *) * (MAX_ARGS + 2));
        for (int i = 0; i < MAX_ARGS + 2; i++){
            argv[i] = malloc(MAX_ARG_LENGTH + 1);
        }
        int argc = 0;
        int command = getFullCommand(str, length, &argc, argv);
        if (command == -1)
        {
            return 0;
        }
        if (commands[command].executable == 0)
        {
            commands[command].function(0, argv);
        }
        execve(commands[command].function, NULL, 0, argv);
        for(int i = 0; i < MAX_ARGS + 2; i++){
            free(argv[i]);
        }
        free(argv);
        return 0;
    }

    Pipe connectingPipe = pipe(NULL, NULL);
    Pipe pipes1[2] = {NULL, connectingPipe};
    Pipe pipes2[2] = {connectingPipe, NULL};
    int pipeQty = 2;

    char **argv1 = malloc(sizeof(char *) * (MAX_ARGS + 2));
    for (int i = 0; i < MAX_ARGS + 2; i++){
        argv1[i] = malloc(MAX_ARG_LENGTH + 1);
    }
    char **argv2 = malloc(sizeof(char *) * (MAX_ARGS + 2));
    for (int i = 0; i < MAX_ARGS + 2; i++){
        argv2[i] = malloc(MAX_ARG_LENGTH + 1);
    }
    int argc = 0;
    int command1 = getFullCommand(str, pipePos,&argc, argv1);
    int command2 = getFullCommand(str + pipePos + 1, length - pipePos - 1, &argc, argv2);
    if (command1 == -1 || command2 == -1)
    {
        return 0;
    }
    if (commands[command1].executable == 0 || commands[command2].executable == 0)
    {
        printText("Error: one of the commands is not executable.\n");
        return 0;
    }

    int pid1 = execve(commands[command1].function, pipes1, pipeQty, argv1);
    int pid2 = execve(commands[command2].function, pipes2, pipeQty,argv2);
 
    waitpid(pid1);
    waitpid(pid2);

    close(connectingPipe);

    for(int i = 0; i < MAX_ARGS + 2; i++){
        free(argv1[i]);
        free(argv2[i]);
    }
    free(argv1);
    free(argv2);
    
    return 0;
}

int getFullCommand(char *str, int length, int *argc, char **args)
{
    char command[length + 1];
    char *arguments[MAX_ARGS];
    for (int i = 0; i < MAX_ARGS; i++)
    {
        arguments[i] = malloc(MAX_ARG_LENGTH + 1);
    }
    *argc = 0;
    char commandName[length + 1];
    memcpy(command, str, length);
    command[length] = 0;
    getCommandAndArgs(command, arguments, argc, commandName, length + 1);
    int commandIndex = getCommandIndex(commandName);
    if (commandIndex == -1){
        return -1;
    }
    memcpy(args[0], commandName, length + 1);
    if (commands[commandIndex].executable){
        int hasBackground = 0;
        if (*argc > 0){
            if (strcmp(args[*argc-1], "&") == 0){
                hasBackground = 1;
                strcpy(args[1], "0");
            }else{
                strcpy(args[1], "1");
            }
        }else{
            strcpy(args[1], "1");
        }
        if (*argc - hasBackground > commands[commandIndex].argMaxQty || *argc + hasBackground < commands[commandIndex].argMinQty){
            printf("Cantidad de argumentos invalida para %s\n", commandName);
            for (int i = 0; i < MAX_ARGS; i++)
            {
                free(arguments[i]);
            }
            free(arguments);
            return -1;
        }

        for (int i = 0; i < *argc; i++){
            strcpy(args[i + 2], arguments[i]);
        }
        args[*argc + 2] = NULL;
    }else{
        if (*argc > commands[commandIndex].argMaxQty || *argc < commands[commandIndex].argMinQty){
            printf("Cantidad de argumentos invalida para %s\n", commandName);
            for (int i = 0; i < MAX_ARGS; i++)
            {
                free(arguments[i]);
            }
            free(arguments);
            return -1;
        }
    }
    for (int i = 0; i < MAX_ARGS; i++)
    {
        free(arguments[i]);
    }
    free(arguments);
    return commandIndex;
}

char getCommandIndex(char *commandName){
    int commandIndex;
    for (commandIndex = 0; commandIndex < COMMAND_QTY; commandIndex++){
        if (strcmp(commandName, commands[commandIndex].name) == 0){
            break;
        }
    }
    if (commandIndex == COMMAND_QTY){
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
    memcpy(command, str+offset, cmd_idx);
    command[cmd_idx] = 0;
    int argIdx = 0;
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
    
    if (argLen > 0)
        (*argQty)++;
    args[*argQty][argLen] = 0;
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
            sendEOF();
            return 0;
        }
        else if (strcmp(argv[0], "all") == 0)
        {
            for (int i = 0; i < COMMAND_QTY; i++)
            {
                printf("%s: %s\n", commands[i].name, commands[i].description);
            }
            sendEOF();
            return 0;
        }
        printf("Argumento invalido para help\n");
    }
    sendEOF();
    return 0;
}


char callFree(char argc, const char** argv)
{
    if (argc == 1 && isHexaNumber(argv[0]))
    {
        char flag = 0;
        uint64_t ptr = hexaStrToNum(argv[0], strlen(argv[0]), &flag);
        if (flag == 1)
        {
            printf("Numero muy grande. Overflow\n");
        }
        else
        {
            const uint64_t freedBytes = free(ptr);
            printf("%x bytes liberados\n", freedBytes);
        }
    }
    else
    {
        printf("Argumento invalido para free\n");
    }
    return 0;
}

char callMalloc(char argc, const char** argv)
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
            uint64_t* ptr = malloc(size);
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

char callKill(char argc, const char** argv)
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

char callGetMemoryStatus(char argc, const char** argv)
{
    if (argc != 0)
    {
        printf("Argumento invalido para get-memory-status\n");
    }
    else
    {
        uint64_t* memStatus = getMemoryStatus();
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

char callBlock(char argc, const char** argv)
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

char callMemoryDump(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty == 1 && isHexaNumber(arguments[0]))
    {
        char flag = 0;
        uint64_t direction = hexaStrToNum(arguments[0], strlen(arguments[0]), &flag);
        if (flag == 1)
        {
            // printerr("Direccion muy grande. Overflow\n", 0);
            printf("Direccion muy grande. Overflow\n");
        }
        else
        {
            memoryDump(direction);
        }
    }
    else
    {
        printf("Argumento invalido para memory-dump\n");
    }
    return 0;
}

char callNice(char argc, const char** argv)
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

char callTestMM(char argc, const char** argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para test-mm\n");
    }
    else
    {
        uint64_t memorySize = strToNum(argv[0], strlen(argv[0]));
        test_mm(memorySize);
    }
    return 0;
}

char setFontSize(char argc, const char** argv)
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

char exitConsole(uint8_t argumentQty, const char** arguments)
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

char callSleep(char argc, const char **argv)
{
    if (argc != 1)
    {
        printf("Argumento invalido para sleep\n");
    }
    else
    {
        int num = strToNum(argv[0], strlen(argv[0]));
        _sys_sleep(num);
    }
    return 0;
}