#ifndef _SYS_CALL_INTERFACE_H_
#define _SYS_CALL_INTERFACE_H_

#include <stdint.h>

typedef struct semaphore *sem_t;

typedef struct PipeCDT *Pipe;

typedef int pid_t;

#define STDERR 2
#define STDOUT 1
#define STDIN 0
#define REGISTER_QTY 17

void _sys_write(int fd, const char *buf, uint64_t count);
int _sys_read(int fd, const char *buf, uint32_t count);
uint8_t _sys_getScreenBpp();
void _sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width * _sys_getScreenBpp() / 8]);
uint32_t _sys_getMillis();
void _sys_cleanScreen();
uint16_t _sys_getScreenWidth();
uint16_t _sys_getScreenHeight();
void _sys_beep(uint16_t frequency);
void _sys_getTime(int *hour, int *min, int *seg);
void _sys_setFontSize(uint8_t size, uint8_t rewrite);
uint8_t _sys_getFontSize();
void _sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color, uint16_t row, uint16_t col);
char _sys_getSavedRegisters(uint64_t registers[REGISTER_QTY]);
void *_sys_malloc(uint64_t size);
uint64_t _sys_free(void *ptr);
int _sys_execve(void *entryPoint, Pipe *pipes, char pipeQty, char *const argv[]);
void _sys_printProcesses(char showKilled);
void _sys_exit(int status);
void _sys_yield();
pid_t _sys_getpid();
pid_t _sys_block(int pid);
sem_t _sys_semOpen(char *name, int value);
void _sys_semClose(sem_t sem);
void _sys_semWait(sem_t sem);
void _sys_semPost(sem_t sem);
uint64_t *_sys_getMemoryStatus();
pid_t _sys_kill(int pid);
int _sys_nice(pid_t pid, int priority);
pid_t _sys_waitpid(pid_t pid);
void *_sys_realloc(void *ptr, uint64_t newSize);
Pipe _sys_openProcessPipe(char *name, int fds[2]);
int _sys_closeProcessPipe(int fd);
void _sys_sleep(int millis);

#endif
