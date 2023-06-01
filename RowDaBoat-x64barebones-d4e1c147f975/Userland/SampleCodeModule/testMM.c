#include <testMM.h>
#include <memory.h>
#include <test_util.h>

#define MAX_BLOCKS 128
#define BLOCK_STRUCT_SIZE 32

typedef struct MM_rq
{
  void *address;
  uint32_t size;
} mm_rq;

int test_mm(uint64_t max_memory)
{
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

  int count = 0;
  while (1)
  {

    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory)
    {
      int uniform = max_memory - total - 1 - BLOCK_STRUCT_SIZE;
      if (uniform <= 0)
      {
        break;
      }
      mm_rqs[rq].size = GetUniform(uniform) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address)
      {
        total += mm_rqs[rq].size + BLOCK_STRUCT_SIZE;
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

        free(mm_rqs[i].address);
      }
    }
    printf("Completed test %d successfully\n", count);
    count++;
  }
  return 0;
}