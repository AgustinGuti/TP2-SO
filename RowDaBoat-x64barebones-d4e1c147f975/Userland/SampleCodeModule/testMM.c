// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

char test_mm(char argc, char *argv[])
{

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
  {
    printf("test_mm: ERROR: Invalid number of arguments\n");
    return 1;
  }

  if ((max_memory = satoi(argv[0])) <= 0)
  {
    printf("test_mm: ERROR: Invalid argument\n");
    return 1;
  }

  int count = 0;
  while (1)
  {

    rq = 0;
    total = 0;

    while (rq < MAX_BLOCKS && total < max_memory)
    {
      #ifdef BUDDY
        int uniform = max_memory - total - 1;
      #else
        int uniform = max_memory - total - 1 - BLOCK_STRUCT_SIZE;
      #endif

      if (uniform <= 0)
      {
        break;
      }
      mm_rqs[rq].size = GetUniform(uniform) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      #ifdef BUDDY
        mm_rqs[rq].size = getNextPowerOfTwo(mm_rqs[rq].size);
      #else
        total += BLOCK_STRUCT_SIZE;
      #endif

      if (mm_rqs[rq].address)
      {
        total += mm_rqs[rq].size;
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
    printf("Test %d completado exitosamente\n", count);
    count++;
  }
  return 0;
}

