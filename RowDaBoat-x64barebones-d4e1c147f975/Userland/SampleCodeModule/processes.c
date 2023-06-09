// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <processes.h>

#define EOF -1

#define IS_VOWEL(c) ((c) == 'a' || (c) == 'e' || (c) == 'i' || (c) == 'o' || (c) == 'u' || (c) == 'A' || (c) == 'E' || (c) == 'I' || (c) == 'O' || (c) == 'U')

int execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[])
{
    return _sys_execve(entryPoint, pipes, pipeQty, argv);
}

char printProcesses(char argc, char **argv)
{
    if (argc == 0)
    {
        _sys_printProcesses(0);
        return 0;
    }
    if (argc == 1 && strcmp(argv[0], "-k") == 0)
    {
        _sys_printProcesses(1);
        return 0;
    }
    printf("Argumentos invalidos\n");
    return 1;
}

int getpid()
{
    return _sys_getpid();
}

void yield()
{
    _sys_yield();
}

void exit(int value)
{
    _sys_exit(value);
}

pid_t blockProcess(int pid)
{
    return _sys_block(pid);
}

pid_t kill(int pid)
{
    return _sys_kill(pid);
}

pid_t waitpid(pid_t pid)
{
    return _sys_waitpid(pid);
}

char cat(char argc, char **argv)
{
    int c;
    while ((c = getChar()) != EOF)
    {
        printf("%c", c);
    };
    return 0;
}
char wc(char argc, char **argv)
{
    int count = 1;
    int c;
    while ((c = getChar()) != EOF)
    {
        if (c == '\n')
        {
            count++;
        }
    }
    printf("\nLineas: %d\n", count);
    return 0;
}

char loop(char argc, char **argv)
{
    int sec = strToNum(argv[0], strlen(argv[0]));
    while (1)
    {
        sleep(sec);
        printf("Hola mundo desde el proceso %d\n", _sys_getpid());
    }
    return 0;
}
char filter(char argc, char **argv)
{
    int c;
    while ((c = getChar()) != EOF)
    {
        if (IS_VOWEL(c))
        {
            printf("%c", c);
        }
    }
    return 0;
}

void sleep(int sec)
{
    _sys_sleep(sec * 1000);
}

char setAutoPrio(char argc, char **argv)
{
    char value = strToNum(argv[0], strlen(argv[0]));
    _sys_setAutoPrio(value);
    return 0;
}