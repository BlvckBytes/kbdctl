#ifndef partial_strdup_h
#define partial_strdup_h

#include <stddef.h>
#include <stdbool.h>

#include "util/mman.h"

/**
 * @brief Partially duplicate a string by specifying an offset and
 * the next separating character sequence. "\0" means until end of string.
 * When specifying "\n" as the separator, preceeding carriage returns will
 * be automatically eaten up.
 * 
 * @param str Full target string
 * @param offs Pointer to the offset, gets updated in place by this routine
 * @param sep Separator character sequence
 * @param skip Whether or not you want to skip this sequence. This will
 * make the routine return NULL and skip the duping for performance sake
 * 
 * @return char* Duplicated partial string
 */
char *partial_strdup(const char *str, size_t *offs, const char* sep, bool skip);

/**
 * @brief Check whether or not the char sequence in question starts at
 * the provided offset within the target string
 * 
 * @param str Target string to check against
 * @param search Char sequence that should start here
 * @param offs Offset to test for in target string
 * 
 * @return true Search begins here
 * @return false Search doesn't begin here
 */
bool is_substr_loc(const char *str, const char *search, size_t offs);

#endif