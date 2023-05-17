#include <memory.h>
#include <stdint.h>

void *malloc(uint64_t size){
    return _sys_malloc(size);
}
