#include "strconv.h"

char *strconv(const wchar_t *str, size_t max_len)
{
  scptr char *res = mman_alloc(sizeof(char), max_len, NULL);
  wcstombs(res, str, max_len);
  return mman_ref(res);
}