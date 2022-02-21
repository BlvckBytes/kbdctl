#include "substr.h"

char *substr(const char *str, size_t start, size_t end)
{
  // Support -1 shorthand for end
  size_t str_len = strlen(str);
  if (end == -1) end = str_len - 1;

  // Start too low or greater than end, error
  if (start < 0 || start > end) return NULL;

  // End index out of range
  if (end >= str_len) return NULL;

  // Allocate for amount of chars plus the termination char
  scptr char *res = mman_alloc(sizeof(char), end - start + 2, NULL);

  // Copy over chars
  size_t res_index = 0;
  for (size_t i = start; i <= end; i++)
    res[res_index++] = str[i];

  // Terminate and return
  res[res_index] = 0;
  return mman_ref(res);
}