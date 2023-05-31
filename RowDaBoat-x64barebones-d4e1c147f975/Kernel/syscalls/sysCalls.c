#include <sysCalls.h>
#include <memory.h>
#include <scheduler.h>
#include <semaphores.h>
#include <process.h>

#define OUT_LETTER_COLOR BLACK // Blanco sobre negro
#define OUT_BACK_COLOR BLACK
#define ERR_LETTER_COLOR RED
#define ERR_BACK_COLOR BLACK

#define PIT_OSCILLATOR_FREQ 1193180 // Frequency of the PIT oscillator:1.193180 MHz

#define READY_CALLS 34 // functions quantity in sysCalls[]
#define REGISTER_QTY 17

// prints until a 0 is found or count is reached
void sys_write(int fd, const char *buf, uint64_t count);
int sys_read(int fd, const char *buf, uint32_t count);
void sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width * getScreenBpp() / 8]);
uint32_t sys_getMillis();
void sys_cleanScreen();
uint16_t sys_getScreenWidth();
uint16_t sys_getScreenHeight();
void sys_beep(uint16_t frequency);
void sys_getTime(int *hour, int *min, int *seg);
void sys_setFontSize(uint8_t size, uint8_t rewrite);
uint8_t sys_getFontSize();
void sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color, uint16_t row, uint16_t col);
uint8_t sys_getScreenBpp();
char sys_getSavedRegisters(uint64_t registers[REGISTER_QTY]);
void *sys_malloc(uint64_t size);
uint64_t sys_free(void *ptr);
int sys_execve(void *entryPoint, Pipe* pipes, char pipeQty, char *const argv[]);
void sys_printProcesses();
void sys_exit(int status);
void sys_yield();
pid_t sys_getpid();
void sys_block(int pid);
sem_t semOpen(char *name, int value);
void semClose(sem_t sem);
void semWait(sem_t sem);
void semPost(sem_t sem);
uint64_t *sys_getMemoryStatus();
void sys_kill(int pid);
int sys_nice(pid_t pid, int priority);
pid_t sys_waitpid(pid_t pid);
void * sys_realloc(void *ptr, uint64_t newSize);
Pipe sys_openProcessPipe(char *name, int fds[2]);
int sys_closeProcessPipe(int fd);
void sys_sleep(int millis);

static uint64_t sysCalls[] = {
    (uint64_t)&sys_write,
    (uint64_t)&sys_read,
    (uint64_t)&sys_drawSprite,
    (uint64_t)&sys_getMillis,
    (uint64_t)&sys_cleanScreen,
    (uint64_t)&sys_getScreenWidth,
    (uint64_t)&sys_getScreenHeight,
    (uint64_t)&sys_beep,
    (uint64_t)&sys_getTime,
    (uint64_t)&sys_setFontSize,
    (uint64_t)&sys_getFontSize,
    (uint64_t)&sys_formatWrite,
    (uint64_t)&sys_getScreenBpp,
    (uint64_t)&sys_getSavedRegisters,
    (uint64_t)&sys_malloc,
    (uint64_t)&sys_free,
    (uint64_t)&sys_kill,
    (uint64_t)&sys_execve,
    (uint64_t)&sys_printProcesses,
    (uint64_t)&sys_exit,
    (uint64_t)&sys_yield,
    (uint64_t)&sys_getpid,
    (uint64_t)&sys_block,
    (uint64_t)&semOpen,
    (uint64_t)&semClose,
    (uint64_t)&semWait,
    (uint64_t)&semPost,
    (uint64_t)&sys_getMemoryStatus,
    (uint64_t)&sys_nice,
    (uint64_t)&sys_waitpid,
    (uint64_t)&sys_openProcessPipe,
    (uint64_t)&sys_closeProcessPipe,
    (uint64_t)&sys_sleep,
    (uint64_t)&sys_realloc
};

extern void _setupSysCalls(int qty, uint64_t functions[]);
extern void _speaker_tune(uint16_t tune);
extern void _speaker_off();
extern void _hlt();

extern uint64_t savedRegisters[REGISTER_QTY];
extern char haveSaved;
extern void saveCurrentRegs();

void setupSysCalls()
{
    _setupSysCalls(READY_CALLS, sysCalls);
}

void sys_write(int fd, const char *buf, uint64_t count)
{
    return writeProcessPipe(fd, buf, count);
}

// Read up to count chars to buf, returns amount of chars
int sys_read(int fd, const char *buf, uint32_t count)
{
    return readProcessPipe(fd, buf, count);
}

void *sys_malloc(uint64_t size)
{
    return malloc(size);
}

uint64_t sys_free(void *ptr)
{
    if (ptr != NULL)
    {
        return free(ptr);
    }
    return 0;
}

void *sys_realloc(void *ptr, uint64_t newSize)
{
    return realloc(ptr, newSize);
}

int sys_execve(void* entryPoint, Pipe *pipes, char pipeQty, char *const argv[])
{
    return execve(entryPoint, pipes, pipeQty, argv);
}

void sys_kill(int pid)
{
    killProcess(pid);
}

pid_t sys_waitpid(pid_t pid)
{
    return waitpid(pid);
}

uint64_t *sys_getMemoryStatus()
{
    return getMemoryStatus();
}

void sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width * getScreenBpp() / 8])
{
    drawSprite(width, height, sprite, (pxlCoord){xTopLeft, yTopLeft});
}

uint32_t sys_getMillis()
{
    _hlt(); // To avoid continuous calls to getMillis()
    return getMillis();
}

void sys_cleanScreen()
{
    clearCharBuffer();
    cleanScreen();
}

// Starts the beeper with the desired frequency. If frequency is 0, turns off the beeper
void sys_beep(uint16_t frequency)
{
    if (frequency == 0)
    {
        _speaker_off();
    }
    else
    {
        _speaker_tone(PIT_OSCILLATOR_FREQ / frequency);
    }
}

void sys_setFontSize(uint8_t size, uint8_t rewrite)
{
    setFontSize(size, rewrite);
}

uint8_t sys_getFontSize()
{
    return getFontSize();
}
uint8_t sys_getScreenBpp()
{
    return getScreenBpp();
}

uint16_t sys_getScreenWidth()
{
    return getScreenWidth();
}
uint16_t sys_getScreenHeight()
{
    return getScreenHeight();
}

void sys_getTime(int *hour, int *min, int *seg)
{
    *hour = get_hour();
    *min = get_minute();
    *seg = get_seconds();
}

void sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color, uint16_t row, uint16_t col)
{
    switch (fd)
    {
    case STDOUT:
        printFormatStringLimited(color, buf, count, row, col);
        break;
    case STDERR:
        printStringLimited(0xFF0000, buf, count);
        break;
    default:
        break;
    }
}

char sys_getSavedRegisters(uint64_t registers[REGISTER_QTY])
{
    if (!haveSaved)
    {
        return 0;
    }
    for (int i = 0; i < REGISTER_QTY; i++)
    {
        registers[i] = savedRegisters[i];
    }
    return 1;
}

void sys_printProcesses()
{
    printProcesses();
}

void sys_exit(int status)
{
    exit(status);
}

void sys_yield()
{
    yield();
}

pid_t sys_getpid()
{
    return getpid();
}

void sys_block(int pid)
{
    blockHandler(pid);
}

sem_t sys_semOpen(char *name, int value)
{
    return semOpen(name, value);
}

void sys_semClose(sem_t sem)
{
    semClose(sem);
}

void sys_semWait(sem_t sem)
{
    semWait(sem);
}

void sys_semPost(sem_t sem)
{
    semPost(sem);
}

int sys_nice(pid_t pid, int priority)
{
    return nice(pid, priority);
}

Pipe sys_openProcessPipe(char *name, int fds[2])
{
    return openProcessPipe(name, fds);
}

int sys_closeProcessPipe(int fd)
{
    closeProcessPipe(fd);
}

void sys_sleep(int millis)
{
    sleep(millis);
}