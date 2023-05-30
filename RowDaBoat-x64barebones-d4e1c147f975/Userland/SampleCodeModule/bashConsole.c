#include <bashConsole.h>
#include <stddef.h>
#include <processes.h>
#include <testProcesses.h>
#include <tests.h>
#include <memory.h>
#include <phylos.h>

#define MAX_ARGS 10
#define MAX_ARG_LENGTH 100
#define MAX_COMMAND_NAME_LENGTH 50

extern void zeroDivision();
extern void displayTime();
char parseAndExecuteCommands(uint8_t *str, int length);
void getCommandAndArgs(char *str, char *args[], int *argQty, char *command, int length);
char getCommandIndex(char *commandName);

char help(char argc, char **argv);
char clean(uint8_t argumentQty, const char** arguments);
char startTron(uint8_t argumentQty, const char** arguments);
char callMemoryDump(uint8_t argumentQty, const char** arguments);
char time(uint8_t argumentQty, const char** arguments);
char callZeroDivision(uint8_t argumentQty, const char** arguments);
char callInvalidOpcode(uint8_t argumentQty, const char** arguments);
char callSetFontSize(uint8_t argumentQty, const char** arguments);
char exitConsole(uint8_t argumentQty, const char** arguments);
char callInforeg(uint8_t argumentQty, const char** arguments);
char callHimnoAlegria(uint8_t argumentQty, const char** arguments);
char callMalloc(uint8_t argumentQty, const char** arguments);
char callFree(uint8_t argumentQty, const char** arguments);
char callExec(uint8_t argumentQty, const char** arguments);
char callPrintProcesses(uint8_t argumentQty, const char** arguments);
char callGetMemoryStatus(uint8_t argumentQty, const char** arguments);
char callBlock(uint8_t argumentQty, const char** arguments);
char callKill(uint8_t argumentQty, const char** arguments);
char callNice(uint8_t argumentQty, const char** arguments);
char callTestMM(uint8_t argumentQty, const char** arguments);
char callCat(uint8_t argumentQty, const char** arguments);
char callTestSync(uint8_t argumentQty, char **arguments);


#define COMMAND_QTY 24

static char *commandNames[COMMAND_QTY] = {"help", "clear", "tron", "memory-dump", "time", "zero-division", "invalid-opcode", "set-font-size", "inforeg", "exit", "himno-alegria", "malloc", "free", "exec", "ps", "mem-status", "block", "kill", "nice", "test-mm", "phylo", "cat", "test-sync"};
static char (*commands[])(uint8_t, char *) = {&help, &cleanScreen, &tron, &callMemoryDump, &time, &callZeroDivision, &callInvalidOpcode, &callSetFontSize, &callInforeg, &exitConsole, &callHimnoAlegria, &callMalloc, &callFree, &callExec, &callPrintProcesses, &callGetMemoryStatus, &callBlock, &callKill, &callNice, &callTestMM, &phylos, &cat, &callTestSync};
static char executableCommands[COMMAND_QTY] = { 1,      1,      1,      1,              1,          1,                  1,                  0,              1,              0,          1,                0,           0,       0,              1,                      1,              0,          0,        0,           1,          1,        1,          1};
static char argMaxQtys[COMMAND_QTY] = {          1,     0,      0,      1,              0,          0,                  0,                  1,              0,              0,          0,                1,           1,       0,              0,                      0,              1,          1,        1,           1,          0,        0,          1};    
static char argMinQtys[COMMAND_QTY] = {          0,     0,      0,      1,              0,          0,                  0,                  1,              0,              0,          0,                1,           1,       0,              0,                      0,              1,          1,        1,           1,          0,        0,          1};
static char *commandDescriptions[COMMAND_QTY] =
    {"Imprime en pantalla los comandos disponibles. Si el argumento identifica a otro comando, explica su funcionamiento.",
     "Vacia la consola.",
     "Ejecuta el juego \"Tron Light Cycles\" para dos jugadores.",
     "Recibe como parametro una direccion de memoria e imprime los 32 bytes de memoria posteriores a la misma.",
     "Imprime en pantalla la hora del sistema.",
     "Genera la excepcion zero division y muestra en pantalla los registros en el momento del error.",
     "Genera la excepcion invalid opcode y muestra en pantalla los registros en el momento del error.",
     "Permite agrandar o achicar la dimension de del texto en pantalla por argumento.",
     "Imprime el valor de los ultimos registros guardados. Para guardar los registros se debe presionar la tecla LCTRL.",
     "Termina la ejecucion de la consola.",
     "Reproduce el himno de la alegria."};

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
        char command[length + 1];
        int command1Length = length;
        char arguments1[MAX_ARGS][MAX_ARG_LENGTH + 1];
        int argumentQty1 = 0;
        char command1Name[command1Length + 1][MAX_COMMAND_NAME_LENGTH + 1];
        memcpy(command, str, command1Length);
        command[command1Length] = '\0';

        getCommandAndArgs(command, arguments1, &argumentQty1, command1Name, command1Length + 1);
        int command1Index = getCommandIndex(command1Name);
        if (command1Index == -1)
        {
            return 0;
        }

        char *argv1[MAX_ARGS + 2];
        for(int i = 0; i < MAX_ARGS + 2; i++){
            argv1[i] = malloc(MAX_ARG_LENGTH + 1);
        }
        argv1[0] = commandNames[command1Index];
        if (executableCommands[command1Index]){
            int hasBackground = 0;
            if (argumentQty1 > 0){
                if (strcmp(arguments1[argumentQty1-1], "&") == 0){
                    hasBackground = 1;
                    strcpy(arguments1[1], "0");
                }else{
                    strcpy(arguments1[1], "1");
                }
            }else{
                strcpy(arguments1[1], "1");
            }
            if (argumentQty1 - hasBackground > argMaxQtys[command1Index] || argumentQty1 + hasBackground < argMinQtys[command1Index]){
                printf("Cantidad de argumentos invalida para %s\n", command1Name);
                return 0;
            }

            for (int i = 0; i < argumentQty1; i++){
                strcpy(argv1[i + 2], arguments1[i]);
            }
            argv1[argumentQty1 + 2] = NULL;

            execve(commands[command1Index], NULL, 0, argv1);
        }else{
            if (argumentQty1 > argMaxQtys[command1Index] || argumentQty1 < argMinQtys[command1Index]){
                printf("Cantidad de argumentos invalida para %s\n", command1Name);
                return 0;
            }
            commands[command1Index](argumentQty1, arguments1);
        }

        return 0;
    }


    // Split the string at the pipe position
    int command1Length = pipePos;
    int command2Length = length - command1Length - 1;

    char command1[command1Length + 1];
    char command2[command2Length + 1];
    memcpy(command1, str, command1Length);
    command1[command1Length] = '\0';
    memcpy(command2, str + pipePos + 1, command2Length);
    command2[command2Length] = '\0';

    char arguments1[MAX_ARGS][MAX_ARG_LENGTH];
    int argumentQty1 = 0;
    char command1Name[command1Length + 1][MAX_COMMAND_NAME_LENGTH + 1];
    getCommandAndArgs(command1, arguments1, &argumentQty1, command1Name, command1Length + 1);
    int command1Index = getCommandIndex(command1Name);
    if (argumentQty1 > argMaxQtys[command1Index] || argumentQty1 < argMinQtys[command1Index]){
        printf("Cantidad de argumentos invalida para %s\n", command1Name);
        return 0;
    }

    char arguments2[MAX_ARGS][MAX_ARG_LENGTH];
    int argumentQty2 = 0;
    char command2Name[command2Length + 1][MAX_COMMAND_NAME_LENGTH + 1];
    getCommandAndArgs(command2, arguments2, &argumentQty2, command2Name, command2Length + 1);
    printf("Pipe found\n");
    int command2Index = getCommandIndex(command2Name);
    if (argumentQty2 > argMaxQtys[command2Index] || argumentQty2 < argMinQtys[command2Index]){
        printf("Cantidad de argumentos invalida para %s\n", command2Name);
        return 0;
    }


    if(command1Index == -1 || command2Index == -1){
        return 0;
    }

    Pipe connectingPipe = NULL;

    if (executableCommands[command1Index] == 1 && executableCommands[command2Index] == 1){
        connectingPipe = pipe(NULL, NULL);
        if (connectingPipe == NULL)
        {
            printf("Error al crear el pipe\n");
            return 0;
        }
    }

    char *argv1[MAX_ARGS + 2];
    for (int i = 0; i < MAX_ARGS + 2; i++){
        argv1[i] = malloc(MAX_ARG_LENGTH + 1);
    }
    strcpy(argv1[0], command1Name);
    if (strcmp(arguments1[argumentQty1-1], "&") == 0){
        strcpy(argv1[1], "0");
    }else{
        strcpy(argv1[1], "1");
    }
    for (int i = 0; i < argumentQty1; i++){
        strcpy(argv1[i + 2], arguments1[i]);
    }
    argv1[argumentQty1 + 2] = NULL;

    char *argv2[MAX_ARGS + 2];
    for (int i = 0; i < MAX_ARGS + 2; i++){
        argv2[i] = malloc(MAX_ARG_LENGTH + 1);
    }
    strcpy(argv2[0], command2Name);
    if (strcmp(arguments2[argumentQty2-1], "&") == 0){
        strcpy(argv2[1], "0");
    }else{
        strcpy(argv2[1], "1");
    }
    for (int i = 0; i < argumentQty2; i++){
        strcpy(argv2[i + 2], arguments2[i]);
    }
    argv2[argumentQty2 + 2] = NULL;

    Pipe pipes1[2] = {NULL, connectingPipe};
    Pipe pipes2[2] = {connectingPipe, NULL};
    char pipeQty = 2;

    execve(commands[command1Index], pipes1, pipeQty, argv1);
    execve(commands[command2Index], pipes2, pipeQty, argv2);
    return 0;
}

char getCommandIndex(char *commandName){
    int commandIndex;
    for (commandIndex = 0; commandIndex < COMMAND_QTY; commandIndex++){
        if (strcmp(commandName, commandNames[commandIndex]) == 0){
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
    printf("Getting command and args %s\n", str);
    int i = 0;
    while (str[i] == ' ')
    {
        i++;
    }
    int offset = i;
    int cmd_idx = 0;
    while (str[i] != ' ' && str[i] != '\0')
    {
        i++;
        cmd_idx++;
    }
    memcpy(command, str+offset, cmd_idx);
    command[cmd_idx+1] = '\0';
    int argIdx = 0;
    *argQty = 0;
    int argLen = 0;
    while (str[i] != '\0')
    {
        if (str[i] == ' ')
        {
            if (argLen > 0)
            {
                args[*argQty][argLen] = '\0';
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
    args[*argQty][argLen] = '\0';    
}

// Returns exit status
char processCommand(uint8_t *str, int length)
{
    char arguments[MAX_ARGS][1024];
    int argQty = 0;
    char command[length + 1];
    getCommandAndArgs(str, arguments, &argQty, command, length + 1);

    for (int i = 0; i < COMMAND_QTY; i++)
    {
        if (strcmp(command, commandNames[i]) == 0)
        {
            return (*commands[i])(argQty, (const char**)arguments);
        }
    }
    printf("Comando no reconocido\n");
    return 0;
}

char callTron(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty == 0)
    {
        printf("No se puede llamar a tron sin argumentos\n");
        return 0;
    }
    else if (argumentQty > 1)
    {
        printf("Demasiados argumentos para tron\n");
        return 0;
    }
    else
    {
        printf("Llamando a %s...\n", arguments[0]);
        return 0;
    }
}

char help(char argc, char **argv)
{
    printf("Help\n");
    if (argc == 0)
    {
        printf("Los comandos disponibles son:\n");
        for (int i = 0; i < COMMAND_QTY; i++)
        {
            printf("%s\n", commandNames[i]);
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
        else if (strcmp(argv[0], "all") == 0)
        {
            for (int i = 0; i < COMMAND_QTY; i++)
            {
                printf("%s: %s\n\n", commandNames[i], commandDescriptions[i]);
            }
            return 0;
        }
        else
        {
            for (int i = 0; i < COMMAND_QTY; i++)
            {
                if (strcmp(argv[0], commandNames[i]) == 0)
                {
                    printf("%s\n", commandDescriptions[i]);
                    return 0;
                }
            }
        }
        printf("Argumento invalido para help\n");
    }
    return 0;
}


char clean(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para clean\n");
    }
    else
    {
        cleanScreen();
    }
    return 0;
}

char startTron(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para tron\n");
    }
    else
    {
        tron();
    }
    return 0;
}

char callExec(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty <= 1)
    {
        char foreground[2] = "1";
        if (argumentQty == 1)
        {
            if (strcmp(arguments[0], "&") == 0)
            {
                strcpy(foreground, "0");
            }
            else
            {
                printf("Argumento invalido para exec\n");
                return 0;
            }
        }
        char *args[3] = {"processA", foreground, NULL};
        pid_t pidA;
        for (int i = 0; i < 2; i++)
        {
            pidA = execve(&processA, NULL, 0, args);
        }
    }
    else
    {
        printf("Argumentos invalidos para exec\n");
    }
    return 0;
}

char callFree(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty == 1 && isHexaNumber(arguments[0]))
    {
        char flag = 0;
        uint64_t ptr = hexaStrToNum(arguments[0], strlen(arguments[0]), &flag);
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

char callMalloc(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty == 1 && isHexaNumber(arguments[0]))
    {
        char flag = 0;
        uint64_t size = hexaStrToNum(arguments[0], strlen(arguments[0]), &flag);
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

char callKill(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para kill\n");
    }
    else
    {
        int pid = strToNum(arguments[0], strlen(arguments[0]));
        kill(pid);
    }
    return 0;
}

char callGetMemoryStatus(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 0)
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
        free(memStatus);
    }
    return 0;
}

char callBlock(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para block. Debe recibir el PID del proceso a bloquear.\n");
    }
    else
    {
        int num = strToNum(arguments[0], strlen(arguments[0]));
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

char callNice(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 2)
    {
        printf("Argumento invalido para nice\n");
    }
    else
    {
        int pid = strToNum(arguments[0], strlen(arguments[0]));
        int priority = strToNum(arguments[1], strlen(arguments[1]));
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

char callCat(uint8_t argumentQty, const char** arguments)
{
    char foreground[2] = "1";
    if (argumentQty == 1)
    {
        if (arguments[1] == '&')
        {
            strcpy(foreground, "0");
        }
        else
        {
            printf("Argumento invalido para cat\n");
            return 0;
        }
    }
    char *args[3] = {"cat", foreground, NULL};
    pid_t pid;
    pid = execve(&cat, NULL, 0, args);
    return 0;
}

char callTestMM(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para test-mm\n");
    }
    else
    {
        uint64_t memorySize = strToNum(arguments[0], strlen(arguments[0]));
        test_mm(memorySize);
    }
    return 0;
}

char callTestSync(uint8_t argumentQty, char **arguments)
{
    if (argumentQty != 2)
    {
        printf("Argumento invalido para test-sync\n");
    }
    else
    {
        test_sync(argumentQty, arguments);
    }
    return 0;
}

char time(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        displayTime();
    }
    return 0;
}

char callZeroDivision(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para zero-division\n");
    }
    else
    {
        zeroDivision();
    }
    return 0;
}

char callInvalidOpcode(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para invalid-opcode\n");
    }
    else
    {
        invalidOpcode();
    }
    return 0;
}

char callSetFontSize(uint8_t argumentQty, const char** arguments)
{
    if (argumentQty < 1)
    {
        printf("Ingrese una fuente entre 1 y 4\n");
    }
    else if (argumentQty > 1)
    {
        printf("Demasiados argumentos\n");
    }
    else
    {
        int num = strToNum(arguments[0], strlen(arguments[0]));
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

char callInforeg(uint8_t argumentQty, const char **arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        printRegs();
    }
    return 0;
}

char callHimnoAlegria(uint8_t argumentQty, const char **arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        himnoAlegria();
    }
    return 0;
}

char callPrintProcesses(uint8_t argumentQty, const char **arguments)
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        printProcesses();
    }
    return 0;
}
