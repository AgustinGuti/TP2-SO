#include <sysCalls.h>
//#include <memoryManager.h>
#include <buddyMemory.h>

#define OUT_LETTER_COLOR BLACK      //Blanco sobre negro
#define OUT_BACK_COLOR BLACK
#define ERR_LETTER_COLOR RED
#define ERR_BACK_COLOR BLACK

#define PIT_OSCILLATOR_FREQ 1193180     //Frequency of the PIT oscillator:1.193180 MHz

#define READY_CALLS 15              //functions quantity in sysCalls[]
#define REGISTER_QTY 17

//prints until a 0 is found or count is reached
void        sys_write(int fd, const char *buf, uint64_t count);
int         sys_read(int fd, const uint16_t *buf, uint32_t count);
void        sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width*getScreenBpp()/8]);
uint32_t    sys_getMillis();
void        sys_cleanScreen();
uint16_t    sys_getScreenWidth();
uint16_t    sys_getScreenHeight();
void        sys_beep(uint16_t frequency);
void        sys_getTime(int* hour, int* min, int*seg);
void        sys_setFontSize(uint8_t size, uint8_t rewrite);
uint8_t     sys_getFontSize();
void        sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color,uint16_t row, uint16_t col);
uint8_t     sys_getScreenBpp();
char        sys_getSavedRegisters(uint64_t registers[REGISTER_QTY]);
void *      sys_malloc(uint64_t size);

static uint64_t sysCalls[] = {  (uint64_t)&sys_write,(uint64_t)&sys_read,(uint64_t)&sys_drawSprite,(uint64_t)&sys_getMillis, 
                                (uint64_t)&sys_cleanScreen, (uint64_t)&sys_getScreenWidth, (uint64_t)&sys_getScreenHeight,
                                (uint64_t)&sys_beep, (uint64_t)&sys_getTime, (uint64_t)&sys_setFontSize, (uint64_t) &sys_getFontSize, 
                                (uint64_t)&sys_formatWrite, (uint64_t)&sys_getScreenBpp, (uint64_t)&sys_getSavedRegisters, (uint64_t)&sys_malloc};

extern void _setupSysCalls(int qty, uint64_t functions[]);
extern void _speaker_tune(uint16_t tune);
extern void _speaker_off();
extern void _hlt();

extern uint64_t savedRegisters[REGISTER_QTY];
extern char haveSaved;
extern void saveCurrentRegs();

//MemoryManagerADT memoryManager;

// void setupSysCalls(){
//     memoryManager = createMemoryManager((void*)0x400000 - MANAGED_MEMORY_SIZE, (void *)0x400000);
//     _setupSysCalls(READY_CALLS, sysCalls);
// }

BuddyADT memoryManager;

#define MEMORY_INITIAL_DIRECTION 0x400000
#define MEMORY_TO_MAP_SIZE 0x10000000 - MEMORY_INITIAL_DIRECTION

void setupSysCalls(){
    //BuddyADT init_buddy(uint64_t size, uint64_t initialDirection, uint64_t memoryForBuddy, uint64_t memoryForBuddyEnd, uint16_t minBlockSize);

    memoryManager = init_buddy((uint64_t)MEMORY_TO_MAP_SIZE, (uint64_t)MEMORY_INITIAL_DIRECTION, (uint64_t)MEMORY_INITIAL_DIRECTION - 1 - calculateRequiredBuddySize((uint64_t)MEMORY_TO_MAP_SIZE), (uint64_t)MEMORY_INITIAL_DIRECTION-1);
    _setupSysCalls(READY_CALLS, sysCalls);
}


void sys_write(int fd, const char *buf, uint64_t count){
    switch (fd){
        case STDOUT:
            printStringLimited(0xFFFFFF,buf,count);
            break;
        case STDERR:
            printStringLimited(0xFF0000,buf,count);
            break;
        default:
            break;
    }
}

//Read up to count chars to buf, returns amount of chars
int sys_read(int fd, const uint16_t *buf, uint32_t count){
    int ans = 0;
    switch (fd){
        case STDIN:
            ans = getBuffer(buf, count);    //read from buffer
            removeFromBuffer(ans);          //removes what was read
            break;
        default:
            break;
    }
    return ans;
}

void * sys_malloc(uint64_t size){
    return allocMemory(memoryManager, size);
}

void sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width*getScreenBpp()/8]){
    drawSprite(width,height,sprite,(pxlCoord){xTopLeft,yTopLeft});
}

uint32_t sys_getMillis(){
    _hlt();                 //To avoid continuous calls to getMillis()
    return getMillis();
}

void sys_cleanScreen(){
    clearCharBuffer();
    cleanScreen();
}

//Starts the beeper with the desired frequency. If frequency is 0, turns off the beeper
void sys_beep(uint16_t frequency){
    if (frequency == 0){
        _speaker_off();
    }else{
        _speaker_tone(PIT_OSCILLATOR_FREQ/frequency);
    }
}

void sys_setFontSize(uint8_t size, uint8_t rewrite){
    setFontSize(size, rewrite);
}

uint8_t sys_getFontSize(){
    return getFontSize();
}
uint8_t sys_getScreenBpp(){
    return getScreenBpp();
}

uint16_t sys_getScreenWidth(){
    return getScreenWidth();
}
uint16_t sys_getScreenHeight(){
    return getScreenHeight();
}

void sys_getTime(int* hour, int* min, int* seg){
    *hour = get_hour();
    *min = get_minute();
    *seg = get_seconds();
}
 
void sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color,uint16_t row, uint16_t col){
    switch (fd){
        case STDOUT:
            printFormatStringLimited(color,buf,count, row, col);
            break;
        case STDERR:
            printStringLimited(0xFF0000,buf,count);
            break;
        default:
            break;
    }
} 

char sys_getSavedRegisters(uint64_t registers[REGISTER_QTY]){
    if (!haveSaved){
      return 0;
    }
    for (int i = 0; i < REGISTER_QTY; i++){
        registers[i] = savedRegisters[i];
    }
    return 1;
}