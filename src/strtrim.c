#include "strtrim.h"

char *strtrim(const char *str)
{
  // Generate intermediate result
  scptr char* ltrimmed = strltrim(str);
  if (!ltrimmed) return NULL;

  // Generate end-result
  scptr char* rltrimmed = strrtrim(ltrimmed);
  if (!rltrimmed) return NULL;

  // Return end-result
  return mman_ref(rltrimmed);
}

char *strrtrim(const char *str)
{
  for (size_t i = strlen(str) - 1; i >= 0; i--)
  {
    // Ignore spaces and unprintables
    if (str[i] <= 32 || str[i] == 127) continue;

    // Return substring from 0 to last non-space char
    return substr(str, 0, i);
  }

  // Only conisted of spaces
  return NULL;
}

char *strltrim(const char *str)
{
  size_t str_len = strlen(str);
  for (size_t i = 0; i < str_len; i++)
  {
    // Ignore spaces and unprintables
    if (str[i] <= 32 || str[i] == 127) continue;

    // Return substring from first non-space char until end
    return substr(str, i, str_len - 1);
  }

  // Only contained spaces
  return NULL;
}