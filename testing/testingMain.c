#include <stdio.h>
#include <test_mm.h>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Invalid amount of parameters\n");
    return -1;
  }
  uint64_t returnValue = test_mm(1, &argv[1]);
  printf("test_mm returned: %d\n", returnValue);
  return 0;
}
