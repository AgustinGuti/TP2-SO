#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <memory.h>
#include <scheduler.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;


static void * const sampleCodeModuleAddress = (void*)0x400000;		//IMPORTANT moved the entry point to make space for the video driver buffer (6 extra Mb needed)
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

extern void saveRegisters();
extern void restoreStack();

#define MEMORY_INITIAL_DIRECTION 0x600000
#define MEMORY_TO_MAP_SIZE 0x8000000 - MEMORY_INITIAL_DIRECTION
 

int main()
{	
	load_idt();
	saveRegisters();
	restoreStack();

    initializeMemoryManager((uint64_t)MEMORY_TO_MAP_SIZE, (uint64_t)MEMORY_INITIAL_DIRECTION, (uint64_t)MEMORY_INITIAL_DIRECTION - 1 - calculateRequiredBuddySize((uint64_t)MEMORY_TO_MAP_SIZE), (uint64_t)MEMORY_INITIAL_DIRECTION-1);
	initScheduler();
	
	createProcess("shell", sampleCodeModuleAddress, 1, 1, NULL);
	triggerTimer();
	//((EntryPoint)sampleCodeModuleAddress)();
	drawRect((pxlCoord){0,0},0x00FF00,getScreenWidth(),getScreenHeight());		//Execution has ended succesfully
	return 0;
}