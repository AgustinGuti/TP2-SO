#ifndef _SYS_CALL_INTERFACE_H_
#define _SYS_CALL_INTERFACE_H_

#include <stdint.h>

typedef struct semaphore *sem_t;

typedef struct PipeCDT *Pipe;


#define STDERR 2
#define STDOUT 1
#define STDIN 0

void _sys_write(int fd, char *str, int length);
void _sys_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
uint32_t _sys_getMillis();
int _sys_read(int fd, char *buf, int count);
uint16_t _sys_getScreenWidth();
uint16_t _sys_getScreenHeight();
void _sys_cleanScreen();
void _sys_beep(uint16_t frequency);
void _sys_getTime(int *hour, int *min, int *seg);
void _sys_setFontSize(uint8_t size, uint8_t rewrite);
uint8_t _sys_getFontSize();
uint8_t _sys_getScreenBpp();
char _sys_getSavedRegisters(uint64_t registers[17]);
void _sys_formatWrite(int fd, const char *buf, uint64_t count, uint32_t color, uint16_t row, uint16_t col);
void _sys_drawSprite(uint16_t xTopLeft, uint16_t yTopLeft, uint16_t width, uint16_t height, uint8_t sprite[height][width * _sys_getScreenBpp() / 8]);
void _sys_exit(int status);
void _sys_yield();
int _sys_getpid();
void _sys_printProcesses();
int _sys_execve(void *entryPoint, Pipe* pipes, char pipeQty, char *const argv[]);
void *_sys_malloc(uint64_t size);
uint64_t _sys_free(void *ptr);
void _sys_block(int pid);
sem_t _sys_semOpen(char *name, int value);
void _sys_semClose(sem_t sem);
void _sys_semWait(sem_t sem);
void _sys_semPost(sem_t sem);
int _sys_nice(int pid, int priority);
uint64_t * sys_getMemoryStatus();
Pipe _sys_openProcessPipe(char *name, int fds[2]);
int _sys_closeProcessPipe(int fd);
void _sys_sleep(int millis);

#endif
