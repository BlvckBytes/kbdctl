#include "util/strclone.h"

char *strclone(char *origin, size_t max_len)
{
  // Validate that string length is within constraints
  size_t len = strlen(origin);
  if (len > max_len) return NULL;

  // Create a "carbon copy"
  scptr char *clone = mman_alloc(sizeof(char), len + 1, NULL);
  for (size_t i = 0; i < len; i++)
    clone[i] = origin[i];
  clone[len] = 0;

  return mman_ref(clone);
}