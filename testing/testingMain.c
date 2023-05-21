#include <stdio.h>
#include <test_mm.h>

int main(int argc, char *argv[])
{

  char *memory = "1000000000";
  uint64_t returnValue = test_mm(1, &memory);
  printf("test_mm returned: %d\n", returnValue);
  return 0;
}
