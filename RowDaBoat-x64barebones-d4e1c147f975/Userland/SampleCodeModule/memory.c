#include <memory.h>
#include <stdint.h>

void *malloc(uint64_t size){
    return _sys_malloc(size);
}

uint64_t free(void *ptr){
    return _sys_free(ptr);
}

uint64_t * getMemoryStatus(){
    return _sys_getMemoryStatus();
}
