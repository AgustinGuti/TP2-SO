// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <videoDriver.h>
#include <interrupts.h>

#define ZERO_EXCEPTION_ID 	0
#define INVALID_OPCODE_ID 	6
#define CANT_REGISTERS 		17

static void zeroDivision(uint64_t *registers);
static void invalidOpcode(uint64_t *registers);
static void other(uint64_t *registers);
static void printRegs(uint64_t *registers);

static const char* registerNames[CANT_REGISTERS] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15","RSP","RIP"
};

static const uint64_t (*exceptions[])() = {&zeroDivision, &other,&other,&other,&other,&other,&invalidOpcode};

void exceptionDispatcher(int exception, uint64_t registers[CANT_REGISTERS]) {
	printf("Exception: %d\n", exception);	
	while (1){};
	(*exceptions[exception])(registers);
	
	if (exception > 6){
		other(registers);
	}
}

static void zeroDivision(uint64_t *registers) {
	newline();
	startPage();
	cleanScreen();
	printString(0xFF0000,"Zero division exception");
	newline();
	printRegs(registers);
	newline();
}

static void invalidOpcode(uint64_t *registers){
	newline();
	startPage();
	cleanScreen();
	printString(0xFF0000,"Invalid Opcode exception");
	newline();
	printRegs(registers);
	newline();
}

static void other(uint64_t *registers){
	newline();
	startPage();
	cleanScreen();
	printString(0xFF0000,"Other exception");
	newline();
	printRegs(registers);
	newline();	
}

static void printRegs(uint64_t * registers){
	for (int i = 0; i < CANT_REGISTERS; i++){
		printf("%s: 0x%x\n",registerNames[i],registers[i]);	
	}
	
}