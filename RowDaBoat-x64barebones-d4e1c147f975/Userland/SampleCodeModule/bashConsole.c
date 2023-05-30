#include <bashConsole.h>
#include <stddef.h>
#include <processes.h>
#include <testProcesses.h>
#include <tests.h>
#include <memory.h>
#include <phylos.h>

extern void zeroDivision();
extern void displayTime();

char setFontSize(uint8_t argQty, char arguments[argQty], uint8_t rewrite);

char help(uint8_t argumentQty, char arguments[argumentQty]);
char clean(uint8_t argumentQty, char arguments[argumentQty]);
char startTron(uint8_t argumentQty, char arguments[argumentQty]);
char callMemoryDump(uint8_t argumentQty, char arguments[argumentQty]);
char time(uint8_t argumentQty, char arguments[argumentQty]);
char callZeroDivision(uint8_t argumentQty, char arguments[argumentQty]);
char callInvalidOpcode(uint8_t argumentQty, char arguments[argumentQty]);
char callSetFontSize(uint8_t argumentQty, char arguments[argumentQty]);
char exitConsole(uint8_t argumentQty, char arguments[argumentQty]);
char callInforeg(uint8_t argumentQty, char arguments[argumentQty]);
char callHimnoAlegria(uint8_t argumentQty, char arguments[argumentQty]);
char callMalloc(uint8_t argumentQty, char arguments[argumentQty]);
char callFree(uint8_t argumentQty, char arguments[argumentQty]);
char callExec(uint8_t argumentQty, char arguments[argumentQty]);
char callPrintProcesses(uint8_t argumentQty, char arguments[argumentQty]);
char callGetMemoryStatus(uint8_t argumentQty, char arguments[argumentQty]);
char callBlock(uint8_t argumentQty, char arguments[argumentQty]);
char callKill(uint8_t argumentQty, char arguments[argumentQty]);
char callBlock(uint8_t argumentQty, char arguments[argumentQty]);
char callNice(uint8_t argumentQty, char arguments[argumentQty]);
char callFork(uint8_t argumentQty, char arguments[argumentQty]);
char callTestMM(uint8_t argumentQty, char arguments[argumentQty]);
char callPhylo(uint8_t argumentQty, char arguments[argumentQty]);
char callCat(uint8_t argumentQty, char arguments[argumentQty]);

#define COMMAND_QTY 23

static char *commandNames[COMMAND_QTY] = {"help", "clear", "tron", "memory-dump", "time", "zero-division", "invalid-opcode", "set-font-size", "inforeg", "exit", "himno-alegria", "malloc", "free", "exec", "ps", "mem-status", "block", "kill", "nice", "fork", "test-mm", "phylo", "cat"};
static char (*commands[])(uint8_t, char *) = {&help, &clean, &tron, &callMemoryDump, &time, &callZeroDivision, &callInvalidOpcode, &callSetFontSize, &callInforeg, &exitConsole, &callHimnoAlegria, &callMalloc, &callFree, &callExec, &callPrintProcesses, &callGetMemoryStatus, &callBlock, &callKill, &callNice, &callFork, &callTestMM, &callPhylo, &callCat};
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
                        exit = processCommand(commandBuffer, commandBufferPos);
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

// Returns exit status
char processCommand(uint8_t *str, int length)
{
    char command[length];
    int i = 0;
    while (str[i] == ' ')
    {
        i++;
    }
    int cmd_idx = 0;
    while (str[i] != ' ' && i < length)
    {
        command[cmd_idx++] = str[i];
        i++;
    }
    command[cmd_idx] = 0;

    char arguments[length];
    uint8_t arg_idx = 0;
    uint8_t argQty = 0;
    while (i < length)
    {
        if (str[i] == ' ')
        {
            argQty++;
        }
        arguments[arg_idx++] = str[i];
        i++;
    }
    arguments[arg_idx] = 0;

    for (int i = 0; i < COMMAND_QTY; i++)
    {
        if (strcmp(command, commandNames[i]) == 0)
        {
            return (*commands[i])(argQty, arguments);
        }
    }
    printf("Comando no reconocido\n");
    return 0;
}

char setFontSize(uint8_t argQty, char arguments[argQty], uint8_t rewrite)
{
    if (argQty < 1)
    {
        printf("Ingrese una fuente entre 1 y 4\n");
    }
    else if (argQty > 1)
    {
        printf("Demasiados argumentos\n");
    }
    else
    {
        int num = strToNum(arguments+1, strlen(arguments+1));
        if (num > 4 || num < 1)
        {
            printf("Ingrese una fuente entre 1 y 4\n");
        }
        else
        {
            _sys_setFontSize(num, rewrite);
        }
    }
    return 0;
}

char help(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty == 0)
    {
        printf("Los comandos disponibles son:\n");
        for (int i = 0; i < COMMAND_QTY; i++)
        {
            printf("%s\n", commandNames[i]);
        }
    }
    else if (argumentQty > 1)
    {
        printf("Demasiados argumentos para help\n");
    }
    else
    {
        if (strcmp(arguments, "please") == 0)
        {
            printf("No.\n");
            return 0;
        }
        else if (strcmp(arguments, "all") == 0)
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
                if (strcmp(arguments, commandNames[i]) == 0)
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

char clean(uint8_t argumentQty, char arguments[argumentQty])
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

char startTron(uint8_t argumentQty, char arguments[argumentQty])
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

char callExec(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty <= 1)
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
                printf("Argumento invalido para exec\n");
                return 0;
            }
        }
        char *args[3] = {"processA", foreground, NULL};
        pid_t pidA;
        for (int i = 0; i < 1; i++)
        {
            pidA = execve(&processA, args);
        }
    }
    else
    {
        printf("Argumentos invalidos para exec\n");
    }
    return 0;
}

char callFree(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty == 1 && isHexaNumber(arguments+1))
    {
        char flag = 0;
        uint64_t ptr = hexaStrToNum(arguments+1, strlen(arguments+1), &flag);
        if (flag == 1)
        {
            // printerr("Numero muy grande. Overflow\n", 0);
            printf("Numero muy grande. Overflow\n");
        }
        else
        {
            const freedBytes = free(ptr);
            printf("%x bytes liberados\n", freedBytes);
        }
    }
    else
    {
        printf("Argumento invalido para free\n");
    }
    return 0;
}

char callMalloc(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty == 1 && isHexaNumber(arguments + 1))
    {
        char flag = 0;
        uint64_t size = hexaStrToNum(arguments + 1, strlen(arguments + 1), &flag);
        if (flag == 1)
        {
            // printerr("Numero muy grande. Overflow\n", 0);
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

char callKill(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para kill\n");
    }
    else
    {
        int pid = strToNum(arguments+1, 1);
        kill(pid);
    }
    return 0;
}

char callGetMemoryStatus(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 0)
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
        free(memStatus);
    }
    return 0;
}

char callBlock(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para block. Debe recibir el PID del proceso a bloquear.\n");
    }
    else
    {
        int num = strToNum(arguments, strlen(arguments));
        blockProcess(num);
    }
    return 0;
}

char callMemoryDump(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty == 1 && isHexaNumber(arguments))
    {
        char flag = 0;
        uint64_t direction = hexaStrToNum(arguments, strlen(arguments), &flag);
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

char callNice(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 2)
    {
        printf("Argumento invalido para nice\n");
    }
    else
    {
        char *arg1 = malloc(5);
        int i = 1;
        while (arguments[i] != ' ' && i < 4)
        {
            arg1[i] = arguments[i];
            i++;
        }
        arg1[i] = 0;
        i++;
        char *arg2 = malloc(5);
        int j = 0;
        while (arguments[i] != ' ' && j < 4)
        {
            arg2[j] = arguments[i];
            i++;
            j++;
        }
        arg2[j] = 0;
        int pid = strToNum(arg1, strlen(arg1));
        int priority = strToNum(arg2, strlen(arg2));
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

char callFork(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para fork\n");
    }
    else
    {
        fork();
        // int pid = fork();
        // if (pid == -1)
        // {
        //     printf("Error al crear el proceso hijo\n");
        // }
        // else if (pid == 0)
        // {
        //     printf("Soy el proceso hijo\n");
        // }
        // else
        // {
        //     printf("Soy el proceso padre. Mi pid es %d y el de mi hijo es %d\n", getpid(), pid);
        // }
    }
    return 0;
}

char callPhylo(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para phylo\n");
    }
    else
    {
        phylos();
    }
    return 0;
}

char callCat(uint8_t argumentQty, char arguments[argumentQty])
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
    pid = execve(&cat, args);
    return 0;
}

char callTestMM(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para test-mm\n");
    }
    else
    {
        uint64_t memorySize = strToNum(arguments + 1, strlen(arguments + 1));
        test_mm(memorySize);
        // char *arg1 = malloc(5);
        // int i = 1;
        // while (arguments[i] == ' ')
        // {
        //     i++;
        // }
        // int j = 0;
        // while (arguments[i] != ' ' && i < 4)
        // {
        //     arg1[j] = arguments[i];
        //     i++;
        //     j++;
        // }
        // arg1[i] = 0;
        // i++;
        // char *arg2 = malloc(5);
        // j = 0;
        // while (arguments[i] != ' ' && j < 4)
        // {
        //     arg2[j] = arguments[i];
        //     i++;
        //     j++;
        // }
        // arg2[j] = 0;
        // char *argv[2] = {arg1, arg2};
        // test_sync(argumentQty, argv);
    }
    return 0;
}

char time(uint8_t argumentQty, char arguments[argumentQty])
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

char callZeroDivision(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        zeroDivision();
    }
    return 0;
}

char callInvalidOpcode(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        invalidOpcode();
    }
    return 0;
}

char callSetFontSize(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 1)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        setFontSize(argumentQty, arguments, 1);
    }
    return 0;
}

char exitConsole(uint8_t argumentQty, char arguments[argumentQty])
{
    if (argumentQty != 0)
    {
        printf("Argumento invalido para time\n");
    }
    else
    {
        return 1;
    }
    return 0;
}

char callInforeg(uint8_t argumentQty, char arguments[argumentQty])
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

char callHimnoAlegria(uint8_t argumentQty, char arguments[argumentQty])
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

char callPrintProcesses(uint8_t argumentQty, char arguments[argumentQty])
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
