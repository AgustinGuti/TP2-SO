#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>
#include <stdio.h>
#include <sysCallInterface.h>

void *malloc(uint64_t size);
uint64_t free(void *ptr);
uint64_t * getMemoryStatus();

#endif /* MEMORY_H_ */