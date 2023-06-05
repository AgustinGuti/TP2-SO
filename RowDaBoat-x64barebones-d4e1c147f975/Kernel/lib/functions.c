// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <functions.h>

void hexToStr(char *char_num, uint64_t num)
{
  int digits = hexNumLength(num);
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

void decToStr(char *char_num, int num)
{
  int digits = decNumLength(num);
  int isNegative = 0;
  if (num < 0)
  {
    num *= -1;
    char_num[0] = '-';
    isNegative = 1;
  }

  for (int i = digits + isNegative - 1; i >= isNegative; --i)
  {
    char_num[i] = num % 10 + '0';
    num /= 10;
  }
  char_num[digits + isNegative] = 0;
}

int hexNumLength(uint64_t num)
{
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

int decNumLength(int num)
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

uint32_t strlen(const char *str)
{
  uint32_t len = 0;
  while (str[len] != 0)
  {
    len++;
  }
  return len;
}

void strcpy(char *dest, const char *src)
{
  memcpy(dest, src, strlen(src) + 1);
}

int strcmp(const char *str1, const char *str2)
{
  for (; *str1 == *str2 && *str1 != '\0'; str1++, str2++)
    ;
  return *str1 - *str2;
}

int strToNum(const char *str, int longitud)
{
  int neg = 0;

  if (str[0] == '-')
  {
    neg = 1;
  }

  int num = 0;

  for (int i = neg; i < longitud; i++)
  {
    num = num * 10 + str[i] - '0';
  }

  if (neg)
  {
    num *= -1;
  }

  return num;
}

int intComparator(void *a, void *b)
{
  return *(int *)a - *(int *)b;
}