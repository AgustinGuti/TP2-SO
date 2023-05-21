#include <mySyscall.h>
#include <myMemory.h>
#include <memoryManager.h>
#include <test_util.h>
#include <test_mm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 128

typedef struct MM_rq
{
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[])
{

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  int count = 0;
  while (1)
  {

    uint64_t memoryManagerSize = calculateRequiredMemoryManagerSize(max_memory);
    void *memoryManager = malloc(memoryManagerSize);
    void *managedMemory = malloc(max_memory + 32);
    MemoryManagerADT memoryManagerADT = createMemoryManager(max_memory, managedMemory, memoryManager, memoryManager + memoryManagerSize);
    rq = 0;
    total = 32;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory)
    {
      int uniform = max_memory - total - 1 - 32;
      if (uniform <= 0)
      {
        break;
      }
      mm_rqs[rq].size = GetUniform(uniform) + 1;
      mm_rqs[rq].address = allocMemory(memoryManagerADT, mm_rqs[rq].size);

      if (mm_rqs[rq].address)
      {
        total += mm_rqs[rq].size + 32;
        rq++;
      }
    }
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
      {
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }
    }
    // Check
    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
      {

        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
        {
          printf("test_mm ERROR\n");
          return -1;
        }
      }
    }

    // Free
    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
      {
        freeMemory(memoryManagerADT, mm_rqs[i].address);
      }
    }
    printf("Completed test %d\n", count++);
    free(memoryManager);
    free(managedMemory);
  }
  return 0;
}
