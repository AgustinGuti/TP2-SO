#include <memoryDump.h>

#define CANT_REGISTERS 17

#define MAX_MEMORY 0x1000000000 // 64 GB mapped in pure64

static uint64_t registers[CANT_REGISTERS];

static const char *registerNames[CANT_REGISTERS] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RSP", "RIP"};

char memoryDump(char argc, char **argv)
{
    if (argc == 0)
    {
        printf("Invalid arguments\n");
        return 1;
    }
    char flag = 0;
    uint64_t direction = (uint64_t)hexaStrToNum(argv[0], strlen(argv[0]), &flag);
    if (flag == 1)
    {
        printf("Invalid arguments\n");
        return 1;
    }

    if (direction <= MAX_MEMORY - 32)
    {
        uint8_t *memoryPointer = (uint8_t *)direction;
        int cantBytes = 32;
        for (int i = 0; i < cantBytes; i++)
        {
            printf("%x ", *(memoryPointer + i));
        }
        printf("\n");
        return 0;
    }
    else
    {
        printf("Direccion de memoria invalida. La direccion maxima es %x \n", MAX_MEMORY - 32);
        return 1;
    }
}

char printRegs(char argc, char **argv)
{
    char ans = _sys_getSavedRegisters(registers);
    if (ans == 0)
    {
        printf("No hay registros guardados. Presione LCTRL para guardar los registros\n");
        return 1;
    }
    for (int i = 0; i < CANT_REGISTERS; i++)
    {
        printf("%s: 0x%x\n", registerNames[i], registers[i]);
    }
    return 0;
}