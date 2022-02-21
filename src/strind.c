#include "strind.h"

long strind(const char *str, const char *search, size_t offs)
{
  // Iterate through the whole string, beginning at the offset
  size_t str_len = strlen(str), search_len = strlen(search);
  for (size_t i = offs; i < str_len; i++)
  {
    // Iterate through the search string
    bool is_seq = true;
    for (size_t j = 0; j < search_len; j++)
    {
      // Chars match up
      if (str[i + j] == search[j]) continue;

      // Mismatch
      is_seq = false;
      break;
    }

    // Not at sequence yet
    if (!is_seq) continue;
    return i;
  }

  // Sequence could not be located
  return -1;
}