#include "util/partial_strdup.h"

bool is_substr_loc(const char *str, const char *search, size_t offs)
{
  // When searching for the string end, wait until end is reached
  if (search[0] == 0) return str[offs] == 0;

  // Search for full sequence at current offset
  for (const char *c = search; *c; c++)
    if (str[(c - search) + offs] != *c) return false;

  // Found
  return true;
}

char *partial_strdup(const char *str, size_t *offs, const char* sep, bool skip)
{
  // No input provided
  if (!str) return NULL;

  size_t str_len = strlen(str), prev_offs = *offs;
  for (size_t i = *offs; i < str_len; i++)
  {
    // Last character, navigate onto NULL
    if (i == str_len - 1) i++;

    // Wait until the separator has been encountered
    else if (!is_substr_loc(str, sep, i)) continue;

    // Skip separator for next call
    *offs = i + strlen(sep);

    // Don't create the substring
    if (skip) return NULL;

    size_t res_len = i - prev_offs;

    // Eat up carriage return that preceds the targetted newline
    if (str[i - 1] == '\r' && sep[strlen(sep) - 1] == '\n')
      res_len--;

    // Allocate a copy
    scptr char *res = mman_alloc(sizeof(char), res_len + 1, NULL);

    // Copy over string content and terminate
    for (size_t j = 0; j < res_len; j++)
      res[j] = str[j + prev_offs];
    res[res_len] = 0;

    return mman_ref(res);
  }

  return NULL;
}