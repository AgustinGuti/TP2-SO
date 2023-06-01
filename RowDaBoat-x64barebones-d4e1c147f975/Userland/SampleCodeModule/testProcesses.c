#include <testProcesses.h>
#include <stdio.h>

#define EOF -1

#define IS_VOWEL(c) (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')

int a[1] = {100};
sem_t sem;

int processA()
{
    _sys_sleep(10000);
    return 0;
}

void processB()
{
    char foreground[2] = "1";
    char *args[3] = {"processC", foreground, NULL};
    pid_t pidC;
    for (int i = 0; i < 2; i++)
    {
        pidC = execve(&processC, NULL, 0, args);
        waitpid(pidC);
    }
    printf("In B\n");
    // while(1);
    return;
}

void processC()
{
    printf("In C\n");
    return;
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