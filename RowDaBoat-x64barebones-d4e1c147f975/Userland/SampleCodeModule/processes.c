#include <processes.h>

#define EOF -1

#define IS_VOWEL(c) (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')

int execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[])
{
    return _sys_execve(entryPoint, pipes, pipeQty, argv);
}

void printProcesses(char argc, char **argv)
{
    if (argc == 0)
    {
        _sys_printProcesses(0);
    }
    else if (argc == 1 && strcmp(argv[0], "-k") == 0)
    {
        _sys_printProcesses(1);
    }
    else
    {
        printf("Invalid arguments\n");
    }
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

void cat()
{
    char buffer[1];
    do
    {
        _sys_read(0, buffer, 1);
        printf("%c", buffer[0]);
    } while (buffer[0] != EOF);
}
void wc()
{
    int count = 1;
    char c;
    while ((c = getChar()) != EOF)
    {
        //  printf("%c", c);
        if (c == '\n')
        {
            count++;
        }
    }
    printf("\nLineas: %d\n", count);
}

void loop(int sec)
{
    while (1)
    {
        _sys_sleep(sec * 1000);
        printf("Hola mundo desde el proceso %d\n", _sys_getpid());
    }
}
void filter()
{
    char c;
    while ((c = getChar()) != EOF)
    {
        if (IS_VOWEL(c))
        {
            printf("%c", c);
        }
    }
}