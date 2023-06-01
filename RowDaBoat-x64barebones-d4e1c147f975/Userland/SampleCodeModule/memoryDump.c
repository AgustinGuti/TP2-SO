#include <memoryDump.h>

#define CANT_REGISTERS 		17

#define MAX_MEMORY 0x1000000000     //64 GB mapped in pure64

static uint64_t registers[CANT_REGISTERS];
void printRegs();

static const char* registerNames[CANT_REGISTERS] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15","RSP","RIP"
};

void memoryDump(uint64_t direction){
    if (direction <= MAX_MEMORY - 32){
        uint8_t * memoryPointer = (uint8_t *)direction;
        int cantBytes=32;
        for (int i = 0; i < cantBytes; i++)
        {
            printf("%x ",*(memoryPointer+i));
        }
        printf("\n");
    }else{
        // printerr("Direccion de memoria invalida. La direccion maxima es %x \n",1,MAX_MEMORY-32);
        printf("Direccion de memoria invalida. La direccion maxima es %x \n",MAX_MEMORY-32);
    }
}
    
void printRegs(){
    char ans = _sys_getSavedRegisters(registers);
    if (ans == 0){
        printf("No hay registros guardados. Presione LCTRL para guardar los registros\n");
    }else{
        for (int i = 0; i < CANT_REGISTERS; i++){
            printf("%s: 0x%x\n",registerNames[i],registers[i]);	
        }
    }
}