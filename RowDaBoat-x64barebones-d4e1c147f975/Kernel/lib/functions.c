#include <functions.h>

void hex_to_str(char *char_num, int num)
{
  int digits = hex_num_length(num);
  if (num < 0)
  {
    num *= -1;
  }

  for (int i = digits - 1; i >= 0; --i)
  {
    char aux = num % 16;
    if (aux < 10)
    {
      char_num[i] = aux + '0';
    }
    else
    {
      char_num[i] = aux - 10 + 'A';
    }
    num /= 16;
  }
  char_num[digits] = 0;
}

void dec_to_str(char *char_num, int num)
{
  int digits = dec_num_length(num);
  if (num < 0)
  {
    num *= -1;
  }

  for (int i = digits - 1; i >= 0; --i)
  {
    char_num[i] = num % 10 + '0';
    num /= 10;
  }
  char_num[digits] = 0;
}

int hex_num_length(int num)
{
  if (num < 0)
  {
    num *= -1;
  }
  int length = 0;
  while (num != 0)
  {
    length++;
    num /= 16;
  }
  if (length == 0)
    length = 1;

  return length;
}

int dec_num_length(int num)
{
  if (num < 0)
  {
    num *= -1;
  }
  int length = 0;
  while (num != 0)
  {
    length++;
    num /= 10;
  }
  if (length == 0)
    length = 1;
  return length;
}

// Returns the n-nth bit of value
char getBit(uint16_t value, int n)
{
  return (value & 1 << (n)) != 0;
}

uint32_t strlen(char *str)
{
  uint32_t len = 0;
  while (str[len] != 0)
  {
    len++;
  }
  return len;
}

void strcpy(char *dest, char *src){
  memcpy(dest, src, strlen(src) + 1);
}